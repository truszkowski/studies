flags = -warn-error A
execs = client server clientopt serveropt


all: $(execs)
	@echo "   *  Gotowe!"


socket.cmi: socket.mli
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

socket.cmo: socket.ml socket.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

socket.cmx: socket.ml socket.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


user.cmi: user.mli
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

user.cmo: user.ml user.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

user.cmx: user.ml user.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


protocol.cmi: protocol.mli
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc $(flags) -c $<

protocol.cmo: protocol.ml protocol.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc $(flags) -c $<

protocol.cmx: protocol.ml protocol.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt $(flags) -c $<


message.cmi: message.mli
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

message.cmo: message.ml message.cmi 
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

message.cmx: message.ml message.cmi 
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


box.cmi: box.mli 
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

box.cmo: box.ml box.cmi message.cmi 
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

box.cmx: box.ml box.cmi message.cmi 
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


client.cmi: client.mli
	@echo "   *  Kompiluje ; $@ : $<"
	@ocamlc -thread $(flags) -c $<

client.cmo: client.ml client.cmi box.cmi message.cmi user.cmi socket.cmi protocol.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

client.cmx: client.ml client.cmi  box.cmi message.cmi user.cmi socket.cmi protocol.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


server.cmi: server.mli
	@echo "   *  Kompiluje ; $@ : $<"
	@ocamlc -thread $(flags) -c $<

server.cmo: server.ml server.cmi box.cmi message.cmi user.cmi socket.cmi protocol.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

server.cmx: server.ml server.cmi box.cmi message.cmi user.cmi socket.cmi protocol.cmi
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


client_exec.cmo: client_exec.ml
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

client_exec.cmx: client_exec.ml  
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


server_exec.cmo: server_exec.ml
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlc -thread $(flags) -c $<

server_exec.cmx: server_exec.ml
	@echo "   *  Kompiluje : $@ : $<"
	@ocamlopt -thread $(flags) -c $<


client: protocol.cmo socket.cmo message.cmo client.cmo client_exec.cmo
	@echo "   *  Kompiluje : $@ : $^"
	@ocamlc -thread $(flags) unix.cma threads.cma $^ -o $@ -cclib -lthreads

server: protocol.cmo user.cmo socket.cmo message.cmo box.cmo server.cmo server_exec.cmo
	@echo "   *  Kompiluje : $@ : $^"
	@ocamlc -thread $(flags) unix.cma threads.cma $^ -o $@  -cclib -lthreads

clientopt: protocol.cmx socket.cmx message.cmx client.cmx client.cmx client_exec.cmx
	@echo "   *  Kompiluje : $@ : $^"
	@ocamlopt -thread $(flags) unix.cmxa threads.cmxa $^ -o $@ -cclib -lthreads

serveropt: protocol.cmx user.cmx socket.cmx message.cmx box.cmx server.cmx server_exec.cmx
	@echo "   *  Kompiluje : $@ : $^"
	@ocamlopt -thread $(flags) unix.cmxa threads.cmxa $^ -o $@ -cclib -lthreads


clean:
	@echo "   *  Czyszcze kompilacje..."
	@rm -f $(execs) *.cm[iox] *.o



