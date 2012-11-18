(* 
 * Modul udostepniajacy zbior cech okreslonych na planszy i
 * przechowywanych w specjalnie zdefiniowanej tablicy
 * haszujacej(+ operacje na niej). Istotne takze parametry do
 * "inteligentnych" funkcji wyzszego poziomu.
 * 
 *)

module type BGAME_AI =
	sig
		type t_field
		type t_board
		type t_move
		type t_position
		type t_feature = (int * t_field list)
		module AI_HashType : 
			sig 
				type t = t_feature
				val equal : t -> t -> bool
				val hash : t -> int
			end
		module AI_Hash :
			sig
				type key = AI_HashType.t
				type 'a t = 'a Hashtbl.Make(AI_HashType).t
				val create : int -> 'a t
				val clear : 'a t -> unit
				val copy : 'a t -> 'a t
				val add : 'a t -> key -> 'a -> unit
				val remove : 'a t -> key -> unit
				val find : 'a t -> key -> 'a
				val find_all : 'a t -> key -> 'a list
				val replace : 'a t -> key -> 'a -> unit
				val mem : 'a t -> key -> bool
				val iter : (key -> 'a -> unit) -> 'a t -> unit
				val fold : (key -> 'a -> 'b -> 'b) -> 'a t -> 'b -> 'b
				val length : 'a t -> int
			end
		val defaultAI : unit -> float AI_Hash.t
		val readAI : string -> float AI_Hash.t
		val writeAI : float AI_Hash.t -> string -> unit
		val ai_loadLessons : string -> (t_board * float) list
		val ai_weights : string
		val ai_features : (t_board -> t_feature) list
		val ai_patterns : float AI_Hash.t
		val ai_scale_step : float
		val ai_scale_evaluate : float
		val ai_deep_search : int
		val ai_alphabetaCut : float 
	end;;
