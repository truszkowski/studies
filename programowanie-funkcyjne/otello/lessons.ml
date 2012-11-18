
if ((Array.length Sys.argv) < 4)
	then print_string ("Usage: "^Sys.argv.(0)^" <file_out> <file_with_lessons> <iterations>\n")
	else Othello.lessons Sys.argv.(1) Sys.argv.(2) (int_of_string Sys.argv.(3))
;;

exit 0;;

