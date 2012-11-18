(* 
 * Sygnatura protokolu komunikacyjnego.
 *
 * Zbior kluczy i wartosci uzywanych do 
 * komunikacji klient-serwer.
 *
 *)

(*module Protocol : *)
(*  sig *)

    (* Klucz zapytania i mozliwe wartosci. *)
    val key_request      : string
    val val_register     : string
    val val_unregister   : string
    val val_on           : string
    val val_off          : string
    val val_ping         : string
    val val_ask          : string
    val val_chat         : string
    val val_receive      : string
    val val_reply        : string
    
    (* Klucz odpowiedzi i mozliwe wartosci. *)
    val key_reply        : string
    val val_ok           : string
    val val_invalid      : string
    val val_already      : string
    val val_notfound     : string
    val val_access       : string
    val val_timeout      : string

    (* Inne klucze. *)
    val key_nick         : string
    val key_password     : string
    val key_session      : string
    val key_description  : string
    val key_to           : string
    val key_from         : string
    val key_available    : string
    val key_message      : string
    val key_id           : string
    val key_about        : string
    val key_size         : string
    val key_time         : string

(*  end;; *)

