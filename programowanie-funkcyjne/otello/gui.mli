(* 
 * Modul odpowiedzialny za czesc przezentowana uzytkownikowi. 
 * Czyli reakcja na klikniecie, wcisniecie klawisza, prezentacje
 * gry czy tez zapis gry do pliku. 
 *
 *)

module type BGAME_GUI =
	sig
		type t_field
		type t_board
		type t_move
		type t_position
		val gui_resolutions : string
		val gui_title : string 
		val gui_key : t_board -> char -> bool -> t_board
		val gui_mouse : t_board -> int * int -> bool -> t_board
		val gui_show : t_board -> bool -> unit
		val gui_save : t_board -> bool -> unit
		val gui_gamelog : string -> unit
	end;;
