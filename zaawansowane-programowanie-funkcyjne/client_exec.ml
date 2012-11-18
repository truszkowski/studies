if ((Array.length Sys.argv) < 2) then
  begin
    print_string ("Uzycie: "^Sys.argv.(0)^" <adres ip serwera>\n");
    exit 1
  end
;;

print_string ("*  Witaj w kliencie komunikatora w wersji 2.0!\n");;
print_string ("*\n");;
flush stdout;;

Client.init Sys.argv.(1) 23444 5.;;

Client.loop ();;
