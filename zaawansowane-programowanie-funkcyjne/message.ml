(* 
 * Modul obslugi wiadomosci tekstowych.
 *
 * Sposob przetrzymywania wiadomosci jako klucz-wartosc.
 * Modul korzysta z Socketow do wysylania i odbierania
 * wiadomosci z/do sieci.
 * 
 *)

(*module Message = *)
(*  struct *)

    type t = (string * string) list;;

    exception Wrong of string;;
    exception NotFound;;

    (* Przetworzenie wiadomosci w napis.
     *)
    let string_of_message msg = 
      let rec make_rec l str = 
        match (l) with 
        (k, v)::t -> 
          (
            if ( 
              String.contains k ' ' || 
              String.contains k '=' ||
              String.contains k '\\' ||
              String.contains v '\\' ) then 
                raise (Wrong ("Bledne wartosci w: "^k^"=\\"^v^"\\"))
            else 
              make_rec t (String.concat "" (str::k::"=\\"::v::"\\ "::[]))
          ) |
        [] -> str
      in make_rec msg ""
    ;;
      
    (* Przetworzenie napisu w wiadomosc.
     *)
    let message_of_string str = 
      let rec make_rec s items = 
        if (String.length s) = 0 then items
        else 
          begin
            try (* 'key1="value 1" ... keyN="value N" ' *)
              let end_of_key = String.index s '='
              in let begin_of_val = String.index s '\\'
              in let end_of_val = String.index_from s (begin_of_val+1) '\\'
              in let first_space = String.index s ' '
              in let begin_of_next = String.index_from s (end_of_val+1) ' '
              in if (
                (not (end_of_key + 1 = begin_of_val)) || 
                (first_space < end_of_key) ||
                (not (end_of_val + 1 = begin_of_next))
              ) then raise (Wrong ("Nie udalo sie sparsowac: "^str))
              else begin
                let nextstr = 
                  String.sub s (begin_of_next+1) ((String.length s) - begin_of_next - 1)
                in let key = 
                  String.sub s 0 end_of_key
                in let value = 
                  String.sub s (begin_of_val+1) (end_of_val-begin_of_val-1)
                in make_rec nextstr ((key,value)::items)
              end
            with Not_found -> raise (Wrong ("Nie udalo sie sparsowac: "^str))
          end
      in make_rec str []
    ;;

    (* Nowa, wiadomosc.
     *)
    let create l = l;;

    (* Pobranie elementu, ktory pasuje do klucza.
     *)
    let get msg key = 
      let rec get_rec l = 
        match (l) with 
        (k,v)::t -> 
          if k = key then v else get_rec t |
        [] -> raise NotFound
      in get_rec msg
    ;;

    (* Pobranie listy elementow, ktore pasuja do klucza.
     *)
    let get_list msg key = 
      let rec get_rec l fnd = 
        match (l) with 
        (k,v)::t -> 
          if k = key then get_rec t (v::fnd) else get_rec t fnd |
        [] -> fnd
      in get_rec msg []
    ;;

    (* Dodanie do listy pary klucz - wartosc.
     *)
    let add msg key value =
      (key, value)::msg
    ;;

    (* Odczyt wiadomosci z polaczenia.
     *)
    let read sock = 
      message_of_string (Socket.read sock)
    ;;

    (* Zapis wiadomosci do polaczenia.
     *)
    let write msg sock = 
      Socket.write sock (string_of_message msg)
    ;;

    (* Serwisowanie polaczenia.
     *)
    let service sock lfn forever = 
      let rec service_rec l msg = 
        match (l) with
        h::t -> 
          if h msg sock then 
            ( if forever then service_rec lfn (read sock) )
          else
            service_rec t msg |
        [] -> raise NotFound
      in service_rec lfn (read sock)
    ;;

(*  end;; *)

