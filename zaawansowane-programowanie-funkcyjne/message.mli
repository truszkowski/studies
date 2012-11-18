(* Sygnatura modulu obslugi wiadomosci tekstowych.
 *
 * Sposob przetrzymywania wiadomosci jako klucz-wartosc.
 * Modul korzysta z Socketow do wysylania i odbierania
 * wiadomosci z/do sieci.
 *
 *)

(*module Message : *)
(*  sig *)
    
    (* Wiadomosc rozumiemy jako pary kluch-wartosc *)
    type t = (string * string) list

    (* Wiadomosc jest bledna *)
    exception Wrong of string
    exception NotFound

    (* konwersja wiadomosci w napis *)
    val string_of_message : t -> string
    (* konwersja napisu w wiadomosc *)
    val message_of_string : string -> t
    (* utworzenie nowej wiadomosci *)
    val create : (string * string) list -> t
    (* pobranie wartosci dla klucza *)
    val get : t -> string -> string
    (* pobranie wszystkich wartosci dla klucza *)
    val get_list : t -> string -> string list
    (* dodanie pary klucz-wartosc do wiadomosci *)
    val add : t -> string -> string -> t
    (* odczyt wiadomosci z polaczenia *)
    val read : Socket.t -> t
    (* zapis wiadomosci do polaczenia *)
    val write : t -> Socket.t -> unit
    (* serwisowanie polaczenia przez modul *)
    val service : Socket.t -> (t -> Socket.t -> bool) list -> bool -> unit

(*  end;; *)
