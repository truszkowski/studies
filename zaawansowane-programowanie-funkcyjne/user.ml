(* 
 * Modul oblugi zbioru uzytkownikow.
 *
 * Przechowywanie uzytkownikow w programie oraz
 * opetacje na nich.
 * 
 *)

(*module User = *)
(*  struct *)
    
    exception NotFound;;
    exception Already;;
    exception Access;;
    exception Failed;;
    exception Illegal;;
    exception Timeout;;

    (* Typ uzytkownika.
     *)
    type t = { 
      nick     : string ;
      pass     : string ;
      sessid   : string ;
      descr    : string ; 
      stamp    : float  ;
    };;

    (* Zbior uzytkownikow 
     *)
    let userlist = ref [];;

    (* Timeout na waznosc sesji *)
    let timeout = ref 30.;;

    (* Ustawienie timeoutu na sesje
     *)
    let set_timeout tout = 
      timeout := tout
    ;;

    (* Pobranie wartosci timeoutu
     *)
    let get_timeout () = 
      !timeout
    ;;

    (* Algorytm losowania identyfikatora sesji
     *)
    let new_session () =
      (string_of_float (Unix.time ())) 
      ^
      (string_of_int (Random.int 1000000000))
    ;;

    (* Ochrona danych 
     *)
    let mutex = Mutex.create ();;

    (* Dopuszczalne znaki w niku/hasle/opisie
     *)
    let legals = 
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890`=~!@#$%^&*()_+[]{}|;:',./<>? "
    ;;

    (* Sprawdzenie poprawnosci napisow
     *)
    let rec check lstr = 
      match (lstr) with
        h::t -> 
          begin
            String.iter (
              fun c -> if not (String.contains legals c) then raise Illegal) h ;
            check t
          end |
        [] -> ()
    ;;

    (* Odczytanie informacji z pliku.
     *)
    let load path = 
      try 
        begin
          Mutex.lock mutex;
          let ch = open_in path
          in let rec load_rec l = 
            try ( 
              let nick = input_line ch 
              in let pass = input_line ch
              in let descr = input_line ch
              in (
                check (nick::pass::descr::[]);
(*              print_string ("Wczytuje "^nick^" "^pass^" "^descr^"\n"); 
                flush stdout;*)
                load_rec ({ 
                  nick=nick; pass=pass; sessid=""; descr=descr; stamp=0.0
                }::l)
              )
            ) with 
            Illegal -> (close_in ch; Mutex.unlock mutex; raise Illegal) |
            End_of_file -> ( close_in ch; Mutex.unlock mutex; l )
          in begin
            userlist := load_rec [] ;
            Mutex.unlock mutex
          end
        end
      with
      _ -> raise Failed
    ;;

    (* Zapisanie informacji do pliku.
     *)
    let save path = 
      try 
        begin
          Mutex.lock mutex;
          let ch = open_out path
          in try (
            List.iter (fun h -> Printf.fprintf ch "%s\n%s\n%s\n" h.nick h.pass h.descr) !userlist ;
            close_out ch;
            Mutex.unlock mutex;
          ) with _ -> ( close_out ch; Mutex.unlock mutex; raise Failed )
        end
      with _ -> ( Mutex.unlock mutex; raise Failed )
    ;;

    (* Rejestruj uzytkownika.
     *)
    let register nick pass =
      begin
        check (nick::pass::[]);
        Mutex.lock mutex;
        try
          if List.exists (fun x -> x.nick = nick) !userlist then 
            begin
              Mutex.unlock mutex; raise Already (* juz jest ! *)
            end
          else 
            begin (* Dodajemy ! *)
              userlist := ({
                nick=nick; pass=pass; sessid=""; descr=""; stamp=0.0
              }::!userlist); 
              Mutex.unlock mutex
            end
        with Already -> ( Mutex.unlock mutex; raise Already )
      end
    ;;

    (* Wyrejestruj uzytkownika.
     *)
    let unregister nick pass = 
      begin
        check (nick::pass::[]);
        Mutex.lock mutex;
        begin
          let now = Unix.time ()
          in try 
            userlist := (
              List.filter (
                fun x -> 
                  if (x.nick = nick) then (* nick Ok ! *)
                    if (x.pass = pass) then (* pass Ok ! *)
                      if (x.stamp +. !timeout) > now then raise Access
                      else false (* user offline Ok ! *)
                    else raise Access
                  else true
              ) !userlist
            )
          with Access -> ( Mutex.unlock mutex ; raise Access ) 
        end;
        Mutex.unlock mutex
      end
    ;;

    (* Znajdz uzytkownika.
     *)
    let get nick = 
      try (
        check (nick::[]);
        Mutex.lock mutex;
        (
          let fnd = List.find (fun x -> x.nick = nick) !userlist
          in ( Mutex.unlock mutex; fnd ) (* Jest ! *)
        )
      ) with (* Brak ! *)
      Not_found -> ( Mutex.unlock mutex; raise NotFound )
    ;;

    (* Zalogowanie
     *)
    let on nick pass =
      let now = Unix.time ()
      in let sid = new_session ()
      in try (
        check (nick::[]);
        Mutex.lock mutex;
        (
          let find_f u = 
            if u.nick = nick then (* nick Ok! *)
              if u.pass = pass then (* pass Ok! *)
                if (u.stamp +. !timeout) < now then true (* Nie ma aktywnej sesji *)
                else raise Already (* Jest jakas sesja *)
              else raise Access (* Zle haslo *)
            else false
          in try
            let fnd = List.find find_f !userlist 
            in 
            begin
              userlist := ({
                nick=nick; pass=pass; sessid=sid; descr=fnd.descr; stamp=now
              }::(List.filter (fun x -> not (x.nick = nick)) !userlist));
              Mutex.unlock mutex;
              sid
            end
          with Not_found -> ( Mutex.unlock mutex; raise NotFound )
        )
      ) with 
      Already -> ( Mutex.unlock mutex; raise Already ) |
      Access -> ( Mutex.unlock mutex; raise Access )

    (* Funcja sprawdzajaca - do uzytku wewnetrzego
     *)
    let user_finder u nick sessid now = 
      if u.nick = nick then (* nick Ok! *)
        if u.sessid = sessid then (* sessid Ok! *)
          if (u.stamp +. !timeout) > now then true
          else raise Timeout (* Przeterminowana sesja *)
        else raise Access
      else false

    (* Wylogowanie 
     *) 
    let off nick sessid =
      try (
        check (nick::[]);
        Mutex.lock mutex;
        (
          let find_f u = 
            if u.nick = nick then (* nick Ok! *)
              if u.sessid = sessid then true (* sessid Ok! *)
              else if u.sessid = "" then true (* sessid wygaslo, czyli na to samo wychodzi *)
              else raise Access (* Nieprawidlowe sessid !! *)
            else false
          in try 
            let fnd = List.find find_f !userlist
            in
            begin
              userlist := ({
                nick=nick; pass=fnd.pass; sessid=sessid; descr=fnd.descr; stamp=0.0
              }::(List.filter (fun x -> not (x.nick = nick)) !userlist));
              Mutex.unlock mutex
            end
          with Not_found -> ( Mutex.unlock mutex; raise NotFound )
        )
      ) with Access -> ( Mutex.unlock mutex; raise Access )

    (* Dotkniecie stampa, zrestartowanie tstampa
     *)
    let touch nick sessid =
      let now = Unix.time ()
      in try (
        check (nick::[]);
        Mutex.lock mutex;
        (
          try 
            let fnd = List.find (fun u -> user_finder u nick sessid now) !userlist
            in
            begin
              userlist := ({
                nick=nick; pass=fnd.pass; sessid=sessid; descr=fnd.descr; stamp=now;
              }::(List.filter (fun x -> not (x.nick = nick)) !userlist));
              Mutex.unlock mutex
            end
          with Not_found -> ( Mutex.unlock mutex; raise NotFound )
        )
      ) with 
      Access -> ( Mutex.unlock mutex; raise Access ) |
      Timeout -> ( Mutex.unlock mutex; raise Timeout )

    (* Zmiana opisu
     *)
    let change nick sessid descr =
      let now = Unix.time ()
      in try (
        check (nick::[]);
        Mutex.lock mutex;
        (
          try 
            let fnd = List.find (fun u -> user_finder u nick sessid now) !userlist
            in
            begin
              userlist := ({
                nick=nick; pass=fnd.pass; sessid=sessid; descr=descr; stamp=now;
              }::(List.filter (fun x -> not (x.nick = nick)) !userlist));
              Mutex.unlock mutex
            end
          with Not_found -> ( Mutex.unlock mutex; raise NotFound )
        )
      ) with 
      Access -> ( Mutex.unlock mutex; raise Access ) |
      Timeout -> ( Mutex.unlock mutex; raise Timeout )

(*  end;; *)

