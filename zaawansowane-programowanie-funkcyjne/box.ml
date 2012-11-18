(*
 * Modul kolejkowania przesylanych wiadomosci.
 *
 * Mozna dodawac wiadomosci do okreslonej kolejki (okreslonej
 * przez klucz=string) a potem z tejze kolejki odczytac.
 *
 *)

(*module Box = *)
(*  struct *)
    
    exception Empty;;

    let boxes = ref [];;
    let mutex = Mutex.create ();;

    let take key = 
      try (
        Mutex.lock mutex;
        let (n, q) = List.find (fun (k, v) -> k = key) !boxes 
        in let top = Queue.take q
        in begin
          if (Queue.length q) > 0 then 
            begin (* Jest cos w kolejce *)
              boxes := ((n, Queue.copy q)::(List.filter (fun (k, v) -> k <> key) !boxes));
              Mutex.unlock mutex;
              top
            end
          else
            begin (* Nie ma nic w kolejce, usuwamy ja *)
              boxes := (List.filter (fun (k, v) -> k <> key) !boxes);
              Mutex.unlock mutex;
              top
            end
        end
      ) with (* Jak nie ma to znaczy, ze pusto... *)
      Not_found -> ( Mutex.unlock mutex; raise Empty ) | 
      Queue.Empty -> ( Mutex.unlock mutex; raise Empty )
    ;;

    let push key task = 
      try (
        Mutex.lock mutex;
        let (n, q) = List.find (fun (k, v) -> k = key) !boxes
        in begin (* Jest taka kolejka, mozna dodawac *)
          Queue.push task q;
          boxes := ((n, Queue.copy q)::(List.filter (fun (k, v) -> not (k = key)) !boxes));
          Mutex.unlock mutex
        end
      ) with 
      Not_found -> (* A jak nie ma to tworzymy i dodajemy *)
        let q = Queue.create ();
        in begin
          Queue.push task q;
          boxes := ((key, Queue.copy q)::(List.filter (fun (k, v) -> not (k = key)) !boxes));
          Mutex.unlock mutex
        end
    ;;

    let length key = 
      try (* Patrzymy ile zadan jest w kolejce *)
        let (_, q) = List.find (fun (k, v) -> k = key) !boxes
        in Queue.length q
      with Not_found -> 0
 
 
(*  end;; *)

