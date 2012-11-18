(* 
 * Sygnatura obslugi zbioru uzytkownikow.
 *
 * Przechowywanie uzytkownikow w programie oraz
 * operacje na nich.
 *
 *)

(*module User : *)
(*  sig *)

    exception NotFound (* Brak takiego uzytkownika       *)
    exception Already  (* Juz zarejestrowano/zalogowano  *)
    exception Access   (* Brak dostepu (autoryzacja)     *)
    exception Failed   (* Nie powodzenie (load/save)     *) 
    exception Illegal  (* Niedozwolone znaki             *)
    exception Timeout  (* Przeterminowana sesja          *)

    (* Typ uzytkownika *)
    type t = { 
      nick     : string ; (* nick      *) 
      pass     : string ; (* haslo     *)
      sessid   : string ; (* sesja     *)
      descr    : string ; (* opis      *)
      stamp    : float  ; (* ostatnio  *)
    }

    (* Okreslenie czasu trwania sesji *)
    val set_timeout : float -> unit
    val get_timeout : unit -> float

    (* Wczytaj uzytkownikow z pliku *)
    val load : string -> unit
    (* Zachowaj uzytkownikow do pliku *)
    val save : string -> unit
    (* Zarejestruj uzytkownika *)
    val register : string -> string -> unit
    (* Wyrejestruj uzytkownika *)
    val unregister : string -> string -> unit
    (* Znajdz uzytkownika *)
    val get : string -> t
    (* Zalowanie sie : nick pass -> sessid *)
    val on : string -> string -> string
    (* Wylogowanie sie : nick sessid *)
    val off : string -> string -> unit
    (* Dotkniecie stampu : nick, sessid *)
    val touch : string -> string -> unit
    (* Zmiana opisu : nick, sessid, descr *)
    val change : string -> string -> string -> unit

(*  end;; *)

