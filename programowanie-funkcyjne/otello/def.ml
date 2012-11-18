(*** D E F ***)

(* Wyjatki *)
exception Event_Empty;;       (* "puste" zdarzenie     *)
exception Event_Exit;;        (* wyjscie z gry         *)
exception Event_Reset;;       (* gra od nowa           *)
exception Event_Back;;        (* cofnij ruch           *)
exception Event_Surrender;;   (* poddaj gre            *)
exception Out_Of_Range;;      (* poza plansza          *)

(* Typy *)
type player = Human | Computer;;                    (* gracz...     *)
type event  = Mouse of int * int | Key of char;;    (* zdarzenie... *)
type result = Win of bool | Remis ;;                (* wynik gry... *)

