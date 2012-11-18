(* 
 * Funktor odpowiedzialny za przebieg gier. Zawiera uniwersaln± obs³uge 
 * zdarzen oraz "wysokopoziomowe" funkcje sztucznej inteligencji. Jak 
 * alfabeta-obciecie czy zastosowanie regresji liniowej do polepszania 
 * funkcji oceniajacej. Funkcje te sa parametryzowane funkcjami z modulu
 * BGAME_AI, tam mo¿na ustalic np g³ebokosc przeszukiwania jak i inne sta³e.
 *
 *)

module type BOARD_GAME = 
	functor (B : BGAME_BOARD) ->
	functor (A : BGAME_AI with 
		type t_field=B.t_field and
		type t_board=B.t_board and
		type t_move=B.t_move and
		type t_position=B.t_position) ->
	functor (G : BGAME_GUI with 
		type t_field=B.t_field and
		type t_board=B.t_board and
		type t_move=B.t_move and
		type t_position=B.t_position) ->
		sig
			val evaluate : B.t_board -> bool -> float
			val bestmove : B.t_board -> bool -> B.t_board
			val getEvent : unit -> event
			val game : bool -> player -> player -> B.t_board -> B.t_board list -> result
			val game_run : unit -> unit                    (* <- exec *)
			val training : string -> int -> unit           (* <- exec *)
			val lessons : string -> string -> int -> unit  (* <- exec *)
			val game_init : unit -> unit
			val game_exit : unit -> unit
			val game_save : B.t_board -> bool -> unit
		end;;
