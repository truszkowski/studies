(*
 * Modul obslugi serwera
 * 
 * 
 * 
 *)


(*module Server = *)
(*  struct *)

    let database = ref "";;

    (* Wypisywanie tego co robi serwer 
     *)
    let print str = 
      begin 
        Printf.printf "%3d << %s\n" (Thread.id (Thread.self ())) str;
        flush stdout
      end
    ;;

    (* Zapis ustawien uzytkownikow
     *)
    let save_settings sg = 
      begin
        print ("Pojawil sie sygnal : "^(string_of_int sg)^", koncze prace");
        User.save !database;
        exit 0
      end
    ;;

    (* Wstepne ustawienia
     *)
    let init dbfile session_tout socket_tout =
      begin
        database := dbfile;
        Random.init (int_of_float (Unix.time ()));
        Socket.init ();
        Socket.set_timeout socket_tout;
        User.set_timeout session_tout;
        (
          try User.load !database
          with User.Failed -> 
            ( print ("Nie udalo sie wczytac bazy danych : "^(!database)^"!") ; exit 1; )
        );
        Sys.set_signal Sys.sigquit (Sys.Signal_handle(save_settings));
        Sys.set_signal Sys.sigterm (Sys.Signal_handle(save_settings));
        Sys.set_signal Sys.sigint  (Sys.Signal_handle(save_settings));
        Sys.set_signal Sys.sighup  (Sys.Signal_handle(save_settings));
      end
    ;;

    (*** Zbior odpowiedzi ***)
    let rplOk = (Protocol.key_reply, Protocol.val_ok)::[];;
    let rplIll = (Protocol.key_reply, Protocol.val_invalid)::[];;
    let rplAlr = (Protocol.key_reply, Protocol.val_already)::[];;
    let rplAcs = (Protocol.key_reply, Protocol.val_access)::[];;
    let rplNfd = (Protocol.key_reply, Protocol.val_notfound)::[];;
    let rplTmt = (Protocol.key_reply, Protocol.val_timeout)::[];;

    (* Odpowiedz rpl na zapytanie rq do polaczenia sock *)
    let answer info sock rq rpl = 
      begin
        print ((Socket.string_of_socket sock)^" - "^info^" - request=\""^(Message.string_of_message rq));
        Message.write rpl sock;
        true
      end
    ;;

    let svc_reg msg sock = 
      if Protocol.val_register <> (Message.get msg Protocol.key_request) then
        false
      else 
        let nick = Message.get msg Protocol.key_nick
        in let pass = Message.get msg Protocol.key_password
        in try
          begin
            User.register nick pass;
            answer "REG - Zarejstrowano" sock msg rplOk;
          end
        with 
        User.Already -> answer "REG - Juz jest taki uzytkownik" sock msg rplAlr |
        User.Illegal -> answer "REG - Niedozwolone znaki" sock msg rplIll
    ;;
    
    let svc_unreg msg sock = 
      if Protocol.val_unregister <> (Message.get msg Protocol.key_request) then
        false
      else 
        let nick = Message.get msg Protocol.key_nick
        in let pass = Message.get msg Protocol.key_password
        in try 
          begin
            User.unregister nick pass;
            answer "UNREG - Wyrejestrowano" sock msg rplOk
          end
        with
        User.Access -> answer "UNREG - Brak dostepu" sock msg rplAcs |
        User.Illegal -> answer "UNREG - Niedozwolone znaki" sock msg rplIll 
    ;;
    
    let svc_on msg sock = 
      if Protocol.val_on <> (Message.get msg Protocol.key_request) then
        false
      else 
        let nick = Message.get msg Protocol.key_nick
        in let pass = Message.get msg Protocol.key_password
        in try 
          let sid = User.on nick pass
          in answer "ON - Zalogowano" sock msg ((Protocol.key_session, sid)::rplOk)
        with
        User.Access -> answer "ON - Brak dostepu" sock msg rplAcs |
        User.Illegal -> answer "ON - Niedozwolone znaki" sock msg rplIll |
        User.Already -> answer "ON - Juz zalogowany" sock msg rplAlr |
        User.NotFound -> answer "ON - Brak uzytkownika" sock msg rplNfd
    ;;
    
    let svc_off msg sock = 
      if Protocol.val_off <> (Message.get msg Protocol.key_request) then
        false
      else 
        let nick = Message.get msg Protocol.key_nick
        in let sess = Message.get msg Protocol.key_session
        in try (
          User.off nick sess;
          answer "OFF - Wylogowano" sock msg rplOk
        ) with
        User.Access -> answer "OFF - Brak dostepu" sock msg rplAcs |
        User.Illegal -> answer "OFF - Niedozwolono znaki" sock msg rplIll |
        User.NotFound -> answer "OFF - Brak uzytkownika" sock msg rplNfd
    ;;
    
    let svc_ping msg sock =
      if Protocol.val_ping <> (Message.get msg Protocol.key_request) then
        false
      else 
        let nick = Message.get msg Protocol.key_nick
        in let sess = Message.get msg Protocol.key_session
        in let descr = Message.get msg Protocol.key_description
        in try (
          User.change nick sess descr;
          answer "PING - Zmieniono opis" sock msg rplOk
        ) with
        User.Access -> answer "PING - Brak dostepu" sock msg rplAcs |
        User.Illegal -> answer "PING - Niedozwolone znaki" sock msg rplIll |
        User.NotFound -> answer "PING - Brak uzytkownika" sock msg rplNfd |
        User.Timeout -> answer "PING - Przeterminowana sesja" sock msg rplTmt 
    ;;
    
    let svc_ask msg sock = 
      if Protocol.val_ask <> (Message.get msg Protocol.key_request) then
        false
      else 
        let nick = Message.get msg Protocol.key_nick
        in let sess = Message.get msg Protocol.key_session
        in let about = Message.get msg Protocol.key_about
        in try 
          let us = User.get about
          in let now = Unix.time ()
          in let rpl = (Protocol.key_nick, us.User.nick)::
            (Protocol.key_available, string_of_bool ((us.User.stamp +. (User.get_timeout ())) > now))::
              (Protocol.key_description, us.User.descr)::rplOk
          in try 
            begin
              User.touch nick sess;
              answer "ASK - Pobrano dane" sock msg rpl
            end 
          with User.NotFound -> answer "ASK - Brak uzytkownika" sock msg rplTmt
        with
        User.Access -> answer "PING - Brak dostepu" sock msg rplAcs | 
        User.Illegal -> answer "PING - Niedozwolone znaki" sock msg rplIll |
        User.NotFound -> answer "PING - Nie znaleziono" sock msg rplNfd |
        User.Timeout -> answer "PING - Przeterminowana sesja" sock msg rplTmt 
    ;;

    let svc_chat msg sock = 
      if Protocol.val_chat <> (Message.get msg Protocol.key_request) then
        false
      else 
        let sender = Message.get msg Protocol.key_from
        in let sess = Message.get msg Protocol.key_session
        in let receiver = Message.get msg Protocol.key_to
        in let txt = Message.get msg Protocol.key_message
        in let now = (string_of_float (Unix.time ()))
        in let id = now^(string_of_int(Random.int 1000000000))
        in try 
          let us = User.get receiver
          in try
            let topush = 
              (Protocol.key_request, Protocol.val_chat)::
                (Protocol.key_from, sender)::(Protocol.key_to, receiver)::
                  (Protocol.key_id, id)::(Protocol.key_message, txt)::
                    (Protocol.key_time, now)::[]
            in begin
              User.touch sender sess;
              Box.push us.User.nick topush;
              answer "CHAT - Przekazano" sock msg ((Protocol.key_id, id)::rplOk)
            end
          with 
          User.Access -> answer "CHAT - Brak dostepu" sock msg rplAcs |
          User.NotFound -> answer "CHAT - Brak uzytkownika" sock msg rplTmt |
          User.Timeout -> answer "CHAT - Przeterminowana sesja" sock msg rplTmt
        with
        User.Illegal -> answer "CHAT - Niedozwolone znaki" sock msg rplIll |
        User.NotFound -> answer "CHAT - Nie znaleziono" sock msg rplNfd
    ;;
    
    let svc_receive msg sock = 
      if Protocol.val_receive <> (Message.get msg Protocol.key_request) then
        false
      else
        let nick = Message.get msg Protocol.key_nick
        in let sess = Message.get msg Protocol.key_session
        in let len = Box.length nick
        in try
          begin
            User.touch nick sess;
            (if not (answer ("RECV - W kolejce "^(string_of_int len)) sock msg
            ((Protocol.key_size, (string_of_int len))::rplOk)) then () else ()
            );
            let rec get_rec n = 
              if n = 0 then true
              else if n < 0 then false
              else 
                let msg = Box.take nick
                in begin
                  print ("RECV - Wysylam : "^(Message.string_of_message msg));
                  Message.write msg sock;
                  let rpl = Message.read sock
                  in begin
                    print ("RECV - Odebralem : "^(Message.string_of_message rpl));
                    get_rec (n-1)
                  end
                end
            in get_rec len
          end
        with
        Box.Empty -> (answer "RECV - Podwojne polaczenie?" sock msg rplIll) |
        User.Access -> (answer "RECV - Brak dostepu" sock msg rplAcs) | 
        User.NotFound -> (answer "RECV - Brak uzytkownika" sock msg rplTmt) |
        User.Timeout -> (answer "RECV - Przeterminowana sesja" sock msg rplTmt) |
        User.Illegal -> (answer "RECV - Niedozwolone znaki" sock msg rplIll) 
    ;;

    let svc_blind msg sock =
      begin
        print ("Nieprawidlowe polecenie, msg = "^(Message.string_of_message msg));
        false
      end
    ;;

    (* Serwisowanie przez message 
     *)
    let service sock lfn =
      try Message.service sock lfn false ; (* nie proboj czytac z lacza w niesk. *)
      with
      Message.NotFound ->
        print ("Nieprawidlowa wiadomosc od "^(Socket.string_of_socket sock)) |
      Message.Wrong e ->
        print ("Blad w wiadomosci("^e^") od "^(Socket.string_of_socket sock)) |
      Socket.Disable -> 
        print ("Nieaktywne polaczenie z "^(Socket.string_of_socket sock)) |
      Socket.Broken ->
        print ("Zerwane polaczenie z "^(Socket.string_of_socket sock)) |
      Socket.Timeout ->
        print ("Przeterminowane polaczenie z "^(Socket.string_of_socket sock)) |
      _ -> (* Tutaj przechwytujemy wszystko inne *)
        print ("Zlapany wyjatek na polaczeniu : "^(Socket.string_of_socket sock))
    ;;

    (* Start obslugi polaczen
     *)
    let start ip port threads = 
      Socket.accept_with 
        service 
        (svc_reg::svc_unreg::svc_on::svc_off::svc_ping::svc_ask::svc_chat::svc_receive::svc_blind::[])
        ip port threads;
    ;;

    (* Auto zapis danych 
     *)
    let autosave () = 
      while true do 
        begin
          Unix.sleep 60;
          User.save (!database^".autosave")
        end
      done
    ;;

(*  end;; *)

