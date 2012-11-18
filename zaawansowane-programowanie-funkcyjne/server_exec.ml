if ((Array.length Sys.argv) < 3) then
  begin
    print_string ("Uzycie: "^Sys.argv.(0)^" <adres ip> <liczba watkow>\n");
    exit 1
  end
;;

(* Ustawienia *)
let db = "users.database";;
let sess_tout = 10.;;
let sock_tout = 3.;;
let ip = Sys.argv.(1);;
let port = 23444;;
let threads = int_of_string Sys.argv.(2);;

(* Ustawienia poczatkowe *)
Server.init db sess_tout sock_tout;;
(* Start serwera *)
Server.start ip port threads;;

print_string ("Serwer uruchomiony!\n");;
flush stdout;;

(* Autozapis *)
Server.autosave ();;
