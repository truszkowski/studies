(* Modul obslugi polaczen tcp.
 *
 * Sockety tworzone sa tak aby dzialaly nieblokujaco. Ponadto
 * na wszystkie operacje (procz acceptow) nalozone sa timeouty. 
 * Jesli wyczerpie sie czas zostanie rzucony wyjatek Timeout.
 *
 * Wywolanie init (), nie jest konieczne ale blokuje ono sygnal
 * SIGPIPE (zatepuje go wyjatek Broken - zerwanie polaczenia z 
 * drugiej strony). 
 *
 *)

(*module Socket = *)
(*  struct *)

    type t = { fd : Unix.file_descr; ip : string ; port : int };;

    exception Timeout;;
    exception Refused;;
    exception Unreach;;
    exception Disable;;
    exception Broken;;
    exception UnExpected;;
    exception CannotListen;;

    (* Opis
     *)
    let string_of_socket s =
      (s.ip^":"^(string_of_int s.port))
    ;;

    (* Domyslny timeout na polaczenia (niewidoczny).
     *)
    let timeout = ref 3.;;

    (* Ustawienie nowego timeoutu.
     *)
    let set_timeout t = begin timeout := t end;;

    (* Pobranie wartosci timeoutu.
     *)
    let get_timeout () = !timeout;;

    (* Inicjalizacja modulu, wylaczenie sygnalu SIGPIPE.
     *)
    let init () = 
      begin 
        Sys.set_signal Sys.sigpipe Sys.Signal_ignore
      end
    ;;

    (* Tworzenie socketu do nasluchu.
     *)
    let listener ip port = 
      try 
        let (fd, addr) = (
          Unix.socket Unix.PF_INET Unix.SOCK_STREAM 0,
          Unix.ADDR_INET(Unix.inet_addr_of_string ip, port)
        ) in try 
          begin
            Unix.bind fd addr; 
            Unix.listen fd 5; 
            {fd= fd; ip= ip; port= port}
          end 
        with _ -> ( Unix.close fd; raise CannotListen )
      with _ -> raise CannotListen
    ;;

    (* Ustanowienie polaczenia.
     *)
    let connect ip port =
      let (fd, addr) = 
        (
          Unix.socket Unix.PF_INET Unix.SOCK_STREAM 0,
          Unix.ADDR_INET(Unix.inet_addr_of_string ip, port) 
          ) in ( (* probujemy sie laczyc przez timeout sek *)
            Unix.set_nonblock fd;
            let rec connect_rec () = 
              try 
                Unix.connect fd addr; { fd= fd; ip= ip; port= port }
              with 
              Unix.Unix_error(Unix.ENOTCONN, _, _) ->
                ( Unix.close(fd); raise Disable ) |
              Unix.Unix_error(Unix.ETIMEDOUT, _, _) ->
                ( Unix.close(fd); raise Timeout ) |
              Unix.Unix_error(Unix.ECONNREFUSED, _, _) -> 
                ( Unix.close(fd); raise Refused ) |
              Unix.Unix_error(Unix.EHOSTUNREACH, _, _) ->
                ( Unix.close(fd); raise Unreach ) |
              Unix.Unix_error(Unix.EINPROGRESS, _, _) -> 
                let (_, wl, _) = Unix.select [] (fd::[]) [] !timeout
                in
                (
                  match (wl) with
                  [] -> raise Timeout |
                  _ -> connect_rec ()
                ) in connect_rec ()
          )
    ;;

    (* Odebranie nowego polaczenia.
     *)
    let accept sock = 
      let (fd, addr) = Unix.accept sock.fd
      in match (addr) with
      Unix.ADDR_INET(ip, port) -> 
        ( Unix.set_nonblock fd; { fd= fd; ip= Unix.string_of_inet_addr ip; port= port } ) |
      _ -> 
        ( Unix.close fd; raise UnExpected )
    ;;


    (* Zamkniecie polaczenia.
     *)
    let close sock =
      Unix.close sock.fd
    ;;

    (*
     * Wysylanie odbywa sie tak:
     * 
     * Wysylamy w 4 znakach dlugosc tego co chcemy wyslac.
     * Wysylamy wlasciwe dane.
     * 
     * Odbieranie odbywa sie tak:
     *
     * Odbieramy ile nalezy przeczytac.
     * Odbieramy wlasciwe dane.
     *
     *)

    (* Odczyt z polaczenia.
     *)
    let read sock = 
      let cnt = String.create 4
      in try (* odczytujemy ile trzeba odczytac i... czytamy *)
        let (rl, _, _) = Unix.select (sock.fd::[]) [] [] !timeout
        in begin
          match (rl) with 
          [] -> 
            raise Timeout |
          _ -> 
            let rd = Unix.read sock.fd cnt 0 4
            in 
            if not (rd = 4) then raise Disable 
            else 
              let len = int_of_string ("0x"^cnt)
              in let str = String.create len
              in let rd = Unix.read sock.fd str 0 len
              in 
              if not (rd = len) then raise Disable 
              else str
        end
      with 
      Unix.Unix_error(Unix.EPIPE, _, _) -> raise Broken |
      Unix.Unix_error(Unix.ECONNREFUSED, _, _) -> raise Refused |
      Unix.Unix_error(Unix.ENOTCONN, _, _) -> raise Disable
    ;;


    (* Zapis do polaczenia.
     *)
    let write sock str = 
      try 
        let rec write_rec str off len = 
          if len > 0 then begin
            let (_, wl, _) = Unix.select [] (sock.fd::[]) [] !timeout
            in begin
              match (wl) with
              [] -> raise Timeout |
              _ -> 
                let wr = Unix.write sock.fd str off len
                in write_rec str (off+wr) (len-wr);
            end
          end
        in let len = String.length str
        in let cnt = Printf.sprintf "%.4x" len 
        in write_rec (cnt^str) 0 (4+len)
      with 
      Unix.Unix_error(Unix.EPIPE, _, _) -> raise Broken |
      Unix.Unix_error(Unix.ECONNREFUSED, _, _) -> raise Refused |
      Unix.Unix_error(Unix.ENOTCONN, _, _) -> raise Disable
    ;;

    (* Do odebranego polaczenia aplikuj funkcje fn.
     *)      
    let accept_with fn arg ip port n = 
      let lstn = listener ip port
      in let rec single () = 
        try 
          let sock = accept lstn
          in begin 
            begin try fn sock arg with _ -> () end;
            close sock;
            single ()
          end
        with _ -> single ()
      in let rec multi n l = 
        if n = 0 then l
        else multi (n-1) ((Thread.create single ())::l)
      in multi n []
    ;;

    (* Do wykonanego polaczenia aplikuj funkcji fn.
     *)
    let connect_with fn arg ip port = 
      let sock = connect ip port
      in try ( fn sock arg; close sock; )
      with
      Disable -> ( close sock; raise Disable ) |
      Refused -> ( close sock; raise Refused ) |
      Unreach -> ( close sock; raise Unreach ) |
      Timeout -> ( close sock; raise Timeout ) |
      Broken  -> ( close sock; raise Broken  ) |
      _       -> ( close sock; raise UnExpected ) 
    ;;


    let writeable sock tout = 
      let (_, wr, _) = Unix.select [] (sock.fd::[]) [] tout
      in wr = (sock.fd::[])
    ;;


    let readable sock tout = 
      let (rd, _, _) = Unix.select (sock.fd::[]) [] [] tout
      in rd = (sock.fd::[])
    ;;

(*  end;; *)

