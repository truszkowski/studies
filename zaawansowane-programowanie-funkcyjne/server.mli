(*
 * Sygnatura modulu obslugi serwera
 *
 * 
 *
 *)

(*module Server : *)
(*  sig *)

    (* Glowne ustawienia serwera *)
    val init : string -> float -> float -> unit
    (* Uruchomienie serwera *)
    val start : string -> int -> int -> Thread.t list
    (* Auto zapis danych (dziala w niesk.) *)
    val autosave : unit -> unit

(*  end;; *)
    
