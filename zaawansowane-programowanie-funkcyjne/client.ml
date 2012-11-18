(*
 * Modul kliencki
 *
 *
 *
 *)

(*module Client *)
(*  struct *)

    let srv_ip = ref "";;
    let srv_port = ref 0;;
    let my_nick = ref "";;
    let my_pass = ref "";;
    let my_sess = ref "";;

    let init ip port tout = 
      begin
        srv_ip := ip;
        srv_port := port;
        Socket.init ();
        Socket.set_timeout tout;
      end
    ;;

    let show str =
      begin
        print_string ("<< "^str^"\n"); 
        flush stdout
      end
    ;;

    (* polacz i przekaz "sterowanie" funkcji f *)
    let send f rq = 
      let fn sock f = 
        begin
          Message.write rq sock;
          let rpl = Message.read sock
          in f sock rpl (* reply na request *)
        end
      in try Socket.connect_with fn f !srv_ip !srv_port
      with (* jest tez plus, ze wystarczy tylko tutaj zrobic przechwytywanie *)
      Socket.Refused -> show ("Serwer odrzucil polaczenie") |
      Socket.Unreach -> show ("Serwer nieosiagalny") |
      Socket.Disable -> show ("Serwer zerwal polaczenie") |
      Socket.Broken -> show ("Serwer zerwal polaczenie") |
      Socket.Timeout -> show ("Przekroczony czas komunikacji z serwerem") |
      Message.Wrong e -> show ("Blad w komunikacji z serwerem: "^e) |
      Message.NotFound -> show ("Blad w komunikacji z serwerem") |
      _ -> show ("Nastapil nieoczekiwany wyjatek");
    ;;

    let receive () = 
      if !my_nick <> "" then (* tylko jesli sess i nick sa zdefiniowane *)
        let rq = (Protocol.key_request, Protocol.val_receive)::
          (Protocol.key_nick, !my_nick)::(Protocol.key_session, !my_sess)::[]
        in let f sock rpl =
          let ans = Message.get rpl Protocol.key_reply
          in if ans = Protocol.val_timeout then
            begin
              show ("Przeterminowana sesja, zaloguj sie ponownie.");
              my_nick := ""; my_pass := ""; my_sess := "";
            end
          else if ans = Protocol.val_ok then
            let len = int_of_string (Message.get rpl Protocol.key_size)
            in let rec f_rec n = 
              if n > 0 then
                let rpl = Message.read sock
                in let fnick = Message.get rpl Protocol.key_from
                in let time = float_of_string (Message.get rpl Protocol.key_time)
                in let txt = Message.get rpl Protocol.key_message
                in let tm = Unix.localtime time
                in begin 
                  Printf.printf "<< %4d:%.2d:%.2d %.2d:%.2d:%.2d %s : %s\n" 
                    (1900+tm.Unix.tm_year) (1+tm.Unix.tm_mon) tm.Unix.tm_mday 
                    tm.Unix.tm_hour tm.Unix.tm_min tm.Unix.tm_sec fnick txt;
                  flush stdout;
                  Message.write ((Protocol.key_reply,Protocol.val_ok)::[]) sock;
                  f_rec (n-1)
                end
            in f_rec len
        in send f rq
    ;;

    let cmd_reg cmd line = 
      if cmd <> "register" then false
      else 
        let (nick, pass) = Scanf.sscanf line "%s %s %s" (fun x y z -> (y, z))
        in let rq = (Protocol.key_request, Protocol.val_register)::
          (Protocol.key_nick, nick)::(Protocol.key_password, pass)::[]
        in let f sock rpl = 
          let ans = Message.get rpl Protocol.key_reply
          in show ans;
        in ( send f rq; true )
    ;;

    let cmd_unreg cmd line = 
      if cmd <> "unregister" then false
      else 
        let (nick, pass) = Scanf.sscanf line "%s %s %s" (fun x y z -> (y, z))
        in let rq = (Protocol.key_request, Protocol.val_unregister)::
          (Protocol.key_nick, nick)::(Protocol.key_password, pass)::[]
        in let f sock rpl = 
          let ans = Message.get rpl Protocol.key_reply
          in show ans;
        in ( send f rq; true )
    ;;

    let cmd_on cmd line = 
      if cmd <> "on" then false
      else if !my_nick <> "" then 
        ( show ("Juz jestes zalogowany jako "^(!my_nick)); true )
      else 
        let (nick, pass) = Scanf.sscanf line "%s %s %s" (fun x y z -> (y, z))
        in let rq = (Protocol.key_request, Protocol.val_on)::
          (Protocol.key_nick, nick)::(Protocol.key_password, pass)::[]
        in let f sock rpl = 
          let ans = Message.get rpl Protocol.key_reply
          in if ans <> Protocol.val_ok then show ans
          else 
            let sess = Message.get rpl Protocol.key_session
            in begin (* dobra! zalogowany *)
              my_nick := nick;
              my_pass := pass;
              my_sess := sess;
              show "Ok"
            end
        in ( send f rq; true )
    ;;

    let cmd_off cmd line =
      if cmd <> "off" then false
      else if !my_nick = "" then
        ( show ("Nie jestes zalogowany"); true )
      else 
        let rq = (Protocol.key_request, Protocol.val_off)::
          (Protocol.key_nick, !my_nick)::(Protocol.key_session, !my_sess)::[]
        in let f sock rpl = 
          let ans = Message.get rpl Protocol.key_reply
          in begin
            show ("Wylogowano, "^ans);
            my_nick := ""; 
            my_pass := ""; 
            my_sess := "";
          end
        in ( send f rq; true )
    ;;

    let cmd_ping cmd line = 
      if cmd <> "ping" then false
      else if !my_nick = "" then
        ( show ("Nie jestes zalogowany"); true )
      else begin
        print_string "wpisz opis: ";
        flush stdout;
        let descr = read_line ()
        in let rq = (Protocol.key_request, Protocol.val_ping)::
          (Protocol.key_nick, !my_nick)::(Protocol.key_session, !my_sess)::
            (Protocol.key_description, descr)::[]
        in let f sock rpl = 
          let ans = Message.get rpl Protocol.key_reply
          in show ans
        in ( send f rq; true )
      end
    ;;

    let cmd_ask cmd line = 
      if cmd <> "ask" then false
      else if !my_nick = "" then
        ( show ("Nie jestes zalogowany"); true )
      else
        let about = Scanf.sscanf line "%s %s" (fun x y -> y)
        in let rq = (Protocol.key_request, Protocol.val_ask)::
          (Protocol.key_nick, !my_nick)::(Protocol.key_session, !my_sess)::
            (Protocol.key_about, about)::[]
        in let f sock rpl = 
          let ans = Message.get rpl Protocol.key_reply
          in if ans <> Protocol.val_ok then show ans
          else
            let descr = Message.get rpl Protocol.key_description
            in let avail = bool_of_string (Message.get rpl Protocol.key_available)
            in if avail then show ("Dostepny, Opis: "^descr)
            else show ("Niedostepny, Opis: "^descr)
        in ( send f rq; true )
    ;;

    let cmd_chat cmd line =
      if cmd <> "chat" then false
      else if !my_nick = "" then 
        ( show ("Nie jestes zalogowany"); true )
      else
        let nick = Scanf.sscanf line "%s %s" (fun x y -> y)
        in begin
          print_string "wpisz tresc: ";
          let txt = read_line ()
          in let now = string_of_float (Unix.time ())
          in let rq = (Protocol.key_request, Protocol.val_chat)::
            (Protocol.key_from, !my_nick)::(Protocol.key_session, !my_sess)::
              (Protocol.key_to, nick)::(Protocol.key_time, now)::
                (Protocol.key_message, txt)::(Protocol.key_time, now)::[]
          in let f sock rpl = 
            let ans = Message.get rpl Protocol.key_reply
            in show ans
          in ( send f rq; true )
        end
    ;;

    let cmd_exit cmd line = 
      if cmd <> "exit" then false
      else
        begin
          print_string ("*  Koniec\n");
          print_string ("*\n");
          flush stdout;
          exit 0;
        end
    ;;

    let cmd_help cmd line = 
      if cmd <> "help" then false
      else 
        begin
          print_string ("*  Menu pomocy\n"); 
          print_string ("*    register <nick> <haslo>\n");
          print_string ("*    unregister <nick> <haslo>\n");
          print_string ("*    on <nick> <haslo>\n");
          print_string ("*    off\n");
          print_string ("*    ping \\ <opis>\n");
          print_string ("*    ask <nick>\n");
          print_string ("*    chat <nick> \\ <wiadomosc>\n");
          print_string ("*    exit\n");
          print_string ("*    help\n");
          print_string ("*\n");
          flush stdout;
          true;
        end
    ;;

    let cmd_wrong cmd line = 
      begin
        print_string ("*  Nieprawidlowe polecenie: "^cmd^"\n");
        print_string ("*    sproboj `help'\n");
        print_string ("*\n");
        flush stdout;
        true;
      end
    ;;

    let rec command () = 
      let (rl, _, _) = Unix.select (Unix.stdin::[]) [] [] 1.0
      in if rl <> [] then
        let line = read_line ()
        in let cmd = Scanf.sscanf line "%s" (fun x -> x)
        in let lfn = (* takie przerabiamy polecenia *)
          cmd_reg::cmd_unreg::cmd_on::cmd_off::cmd_ping::cmd_ask::cmd_chat::cmd_help::cmd_exit::cmd_wrong::[]
        in let rec cmd_rec l = 
          match (l) with 
          h::t -> if (h cmd line) then (command ()) else (cmd_rec t) |
          [] -> command ()
        in cmd_rec lfn
    ;;

    let loop () =
      let rec rcv_loop () = 
        ( receive (); Unix.sleep 1; rcv_loop () )
      in let rec cmd_loop () =
        ( command (); cmd_loop () )
      in let id = Thread.create rcv_loop ()
      in try cmd_loop () 
      with _ -> Thread.kill id
    ;;

(*  end;; *)

