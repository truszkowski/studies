(*
 * Sygnatura modulu kolejkowania przesylanych wiadomosci.
 *
 * Mozna dodawac wiadomosci do okreslonej kolejki (okreslonej
 * przez klucz=string) a potem z tejze kolejki odczytac.
 *
 *)

(*module Box : *)
(*  sig *)
  
    (* Jest pusto *)
    exception Empty

    (* Pobierz zadanie z kolejki o kluczu key *)
    val take : string -> Message.t
    (* Wloz zadanie do kolejki o kluczu key *)
    val push : string -> Message.t -> unit
    (* Rozmiar kolejki o kluczu key *)
    val length : string -> int
  
(*  end;; *)

