(* 
 * Sygnatura modulu obslugi polaczen tcp.
 *
 * Zestaw operacji na polaczeniach jak odczyt i zapis.
 * Takze laczenie sie oraz nasluchiwanie.
 *
 *)

(*module Socket : *)
(*  sig *)
    type t = { fd : Unix.file_descr ; ip : string ; port : int }

    (* wyjatki na polaczeniach *)
    exception Timeout
    exception Refused
    exception Unreach
    exception Disable
    exception Broken
    exception UnExpected
    exception CannotListen

    (* opis*)
    val string_of_socket : t -> string
    (* ustawienie timeoutu na polaczenia *)
    val set_timeout : float -> unit
    (* pobranie timeoutu na polaczenia *)
    val get_timeout : unit -> float
    (* ustawienia poczatkowe *)
    val init : unit -> unit
    (* pobranie socketu do nasluchu *)
    val listener : string -> int -> t
    (* ustanowienie polaczenia *)
    val connect : string -> int -> t
    (* aplikuj funkcje gdy wykona sie polaczenie *)
    val connect_with : (t -> 'a -> unit) -> 'a -> string -> int -> unit
    (* odebranie polaczenia *)
    val accept : t -> t
    (* aplikuj funkcje gdy nadejdzie polaczenie *)
    val accept_with : (t -> 'a -> unit) -> 'a -> string -> int -> int -> Thread.t list
    (* zamkniecie polaczenia *)
    val close : t -> unit
    (* odczyt z polaczenia *)
    val read : t -> string
    (* zapis do polaczenia *)
    val write : t -> string -> unit
    (* czy mozna pisac *)
    val writeable : t -> float -> bool
    (* czy mozna czytac *)
    val readable : t -> float -> bool

(*  end;; *)

