(* 
 * Modul udostepniajacy podstawowe operacje na planszy.
 *
 *)

module type BGAME_BOARD = 
	sig
		type t_field = int
		type t_board = { b : t_field list list ; c: int ; fp : int ; sp : int }
		type t_move = { mx : int ; my : int }
		type t_position = { px : int ; py : int } 
		val startBoard : unit -> t_board
		val iterator : (t_position -> t_field -> unit) -> t_board -> unit 
		val move : t_board -> t_move -> bool -> t_board
		val pass : t_board -> bool -> t_board
		val legal : t_board -> bool -> t_move list
		val isLegal : t_board -> t_move -> bool -> bool
		val isEndGame : t_board -> bool
		val gameResult : t_board -> result
		val invert : t_board -> t_board 
	end;;
