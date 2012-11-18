| package |
package := Package name: 'po'.
package paxVersion: 0;
	basicComment: ''.


package classNames
	add: #Budowlany;
	add: #Budowniczy;
	add: #Dentystyczny;
	add: #Dziura;
	add: #Gra;
	add: #GraMapaPresenter;
	add: #GraMapaView;
	add: #GraPresenter;
	add: #Legowisko;
	add: #Magazyn;
	add: #Mina;
	add: #Mur;
	add: #Nadzwyczajny;
	add: #Plansza;
	add: #Pole;
	add: #Poza;
	add: #Pulapka;
	add: #Rycerz;
	add: #Skoczek;
	add: #Teleport;
	add: #Wieczny;
	add: #Wspolrzedne;
	add: #Wyposazony;
	add: #Zjadacz;
	add: #Zjadalny;
	add: #Zwykle;
	yourself.

package binaryGlobalNames: (Set new
	yourself).

package globalAliases: (Set new
	yourself).

package allResourceNames: (Set new
	add: #GraMapaPresenter -> 'GraMapaView';
	add: #GraPresenter -> 'GraView';
	yourself).

package setPrerequisites: (IdentitySet new
	add: 'Dolphin Smalltalk 5.1\Object Arts\Dolphin\Base\Dolphin';
	add: 'Dolphin Smalltalk 5.1\Object Arts\Dolphin\MVP\Base\Dolphin MVP Base';
	add: 'Dolphin Smalltalk 5.1\Object Arts\Dolphin\MVP\Type Converters\Dolphin Type Converters';
	yourself).

package!

"Class Definitions"!

Object subclass: #Gra
	instanceVariableNames: 'plansza rycerzeZywi rycerzeMartwi rycerzeOk value ilumadojsc zapis'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Object subclass: #Plansza
	instanceVariableNames: 'pola x y'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Object subclass: #Pole
	instanceVariableNames: 'wsp ktojest czywidoczne wygladwid wygladniewid plansza obrazki'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Object subclass: #Rycerz
	instanceVariableNames: 'wyglad stan pole obrazki tura'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Object subclass: #Wspolrzedne
	instanceVariableNames: 'x y'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pole subclass: #Legowisko
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pole subclass: #Magazyn
	instanceVariableNames: 'pojemnosc ilejest licznik czasreaktywacji'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pole subclass: #Mur
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pole subclass: #Poza
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: 'istniejacyobiekt'!
Pole subclass: #Pulapka
	instanceVariableNames: 'licznik wygladnieakt czasreaktywacji'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pole subclass: #Teleport
	instanceVariableNames: 'dokad'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pole subclass: #Zwykle
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Magazyn subclass: #Budowlany
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Magazyn subclass: #Dentystyczny
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Mur subclass: #Wieczny
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Mur subclass: #Zjadalny
	instanceVariableNames: 'wygladruiny czasreaktywacji licznik'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pulapka subclass: #Dziura
	instanceVariableNames: 'szerokosc'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Pulapka subclass: #Mina
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Presenter subclass: #GraMapaPresenter
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Shell subclass: #GraPresenter
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Rycerz subclass: #Nadzwyczajny
	instanceVariableNames: 'cecha'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Rycerz subclass: #Skoczek
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Rycerz subclass: #Wyposazony
	instanceVariableNames: 'iloscekw maxekw'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Wyposazony subclass: #Budowniczy
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
Wyposazony subclass: #Zjadacz
	instanceVariableNames: ''
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!
View subclass: #GraMapaView
	instanceVariableNames: 'wysokosc szerokosc plansza rycerz opisX opisY rozmiar'
	classVariableNames: ''
	poolDictionaries: ''
	classInstanceVariableNames: ''!

"Global Aliases"!


"Loose Methods"!

"End of package definition"!

"Source Globals"!

"Classes"!

Gra guid: (GUID fromString: '{07A99F05-F210-4B4A-A1D1-2E008EC900FF}')!
Gra comment: ''!
!Gra categoriesForClass!Unclassified! !
!Gra methodsFor!

aktualizujRycerzy
	rycerzeZywi keysDo: [ :key | ((rycerzeZywi at: key) stan = 0) ifTrue: [rycerzeMartwi at: (rycerzeMartwi size + 1) put: (rycerzeZywi at: key). rycerzeZywi removeKey: key].].
	rycerzeZywi keysDo: [ :key | ((rycerzeZywi at: key) stan = 2) ifTrue: [rycerzeOk at: (rycerzeOk size + 1) put: (rycerzeZywi at: key). rycerzeZywi removeKey: key].].
!

czyKoniecGry
	ilumadojsc > ((rycerzeZywi size) + (rycerzeOk size))
		ifTrue: [self trigger: #koniecGry with: 0.^true].
	
	ilumadojsc = (rycerzeOk size)
		ifTrue: [self trigger: #koniecGry with: 1.^true].
	
	^false!

czyKoniecTury
	|tmp w|
	w := true.
	rycerzeZywi do: [ :r | tmp := r tura].
	rycerzeZywi do: [ :r | tmp = r tura ifFalse: [w := false]].
	^w







!

dodajR: rycerz pocz: pole
	|s ryc|
	s := rycerzeZywi size.
	rycerzeZywi at: (s + 1) put: rycerz.
	pole dodajR: rycerz.!

initialize
	value := ''.
	rycerzeZywi := Dictionary new.
	rycerzeMartwi := Dictionary new.
	rycerzeOk := Dictionary new.
	ilumadojsc := nil.
	plansza := nil.!

ktoryRycerz
	|ryc k|
	k := 1000000000000.
	rycerzeZywi do: [ :r | (r tura < k) ifTrue: [k := r tura. ryc := r].].
	^ryc.!

opisX: x Y: y
	self value: (plansza wspX: x Y: y) wypisz.!

pokaz
	^plansza!

pominTure
	|max|
	plansza isNil
		ifTrue: [^self].
	
	max := -1.
	rycerzeZywi do: [ :r | (max > (r tura)) ifFalse: [max := r tura]].
	rycerzeZywi do: [ :r | r tura: max].
	self tiktak
		ifTrue: [self trigger: #zmianaR with: (self ktoryRycerz).]
		ifFalse: [self trigger: #zmianaR with: nil].

!

przypisz: dane
	|rozm x y wsp poczx poczy ryc ky kx pola str s stream plik pocz tresc rycerze ptmp|
	
	"zapisywanie danych na odpowiednich atrybutach, inicjowanie odpowiednich obiektow"
	y := (dane at: 1) asString asNumber.
	x := (dane at: 2) asString asNumber.

	plansza := Plansza new.
	plansza 
		x: x;
		y: y.

	ky := 1.
	[ky <= y] whileTrue: [
		pola := (dane at: (ky + 2)) subStrings: ';'.
		kx := 1.
		pola do: [ :p | (p leftString: 1).
		((p leftString: 1) = 'Z') 
			ifTrue: [plansza dodaj: (Zwykle newPl: plansza X: kx Y: ky)].
		((p leftString: 1) = 'J')
			ifTrue: [plansza dodaj: (Zjadalny newPl: plansza X: kx Y: ky)].
		((p leftString: 1) = 'W')
			ifTrue: [plansza dodaj: (Wieczny newPl: plansza X: kx Y:ky )].
		(((p leftString: 1) = 'M') & (p size = 1))
			ifTrue: [plansza dodaj: (Mina newPl: plansza X: kx Y: ky)].
		((p leftString: 1) = 'T')
			ifTrue: [ptmp := p subStrings: 'T'. ptmp := (ptmp at: 2) subStrings: ','.
				plansza dodaj: (Teleport newPl: plansza X: kx Y: ky dokad: (Array with: ((ptmp at: 1) asNumber) with: ((ptmp at: 2) asNumber)))].
		((p leftString: 1) = 'D')
			ifTrue: [plansza dodaj: (Dziura newPl: plansza X: kx Y: ky szerokosc: (((p subStrings: 'D') at: 2) asNumber))].
		((p leftString: 1) = 'C')
			ifTrue: [plansza dodaj: (Budowlany newPl: plansza X: kx Y: ky poj: (((p subStrings: 'C') at: 2) asNumber))].
		(((p leftString: 1) = 'M') & (p size = 2))
			ifTrue: [plansza dodaj: (Dentystyczny newPl: plansza X: kx Y: ky poj: (((p subStrings: 'M') at: 2) asNumber) )].
		((p leftString: 1) = 'L')
			ifTrue: [plansza dodaj: (Legowisko newPl: plansza X: kx Y: ky)].
		kx := kx + 1.
		].
		ky := ky + 1.
	].
	
	
	wsp := (dane at: (y + 3)) subStrings: ';'.
	poczy := (wsp at: 1) asNumber. 
	poczx := (wsp at: 2) asNumber.
	
	ryc := (dane at: (y + 4)) subStrings: ';'.
		
	ilumadojsc := (dane at: (y + 5)) asNumber.
	pocz := (plansza wspX: poczx Y: poczy).
	ryc do: [ :r | 
	((r leftString: 1) = 'N')
		ifTrue: [self dodajR: (Nadzwyczajny new: pocz) pocz: pocz.].
	((r leftString: 1) = 'S')
		ifTrue: [self dodajR: (Skoczek new: pocz) pocz: pocz.].
	((r leftString: 1) = 'B')
		ifTrue: [self dodajR: (Budowniczy new: pocz il: (((r subStrings: 'B') at: 2)) asNumber) pocz: pocz.].
	((r leftString: 1) = 'Z')
		ifTrue: [self dodajR: (Zjadacz new: pocz il: (((r subStrings: 'Z') at: 2)) asNumber) pocz: pocz.].
	].

	rycerze := ''.
	 (rycerzeZywi do: [ :r | rycerze := rycerze , (r wypisz)]).
	tresc := 'Wczytano gre.

rozmiar planszy: ' , (y printString) , 'x' , (x printString) , '

rycerze: 

' , rycerze , '
Do legowiska musi dojsc: ' , (ilumadojsc printString).
	MessageBox notify: tresc.

	^true!

ruszOd: odx od: ody do: dox do: doy
	|ruchx ruchy kierunek w nowyR|
	ruchx := odx - dox.
	ruchy := ody - doy.
	(((odx = dox) | (ody = doy)) & (((ruchx abs) < 3) & ((ruchy abs) < 3)))
 		ifFalse: [^false].

	(ruchx > 0)
		ifTrue: [kierunek := 4].
	(ruchx < 0)
		ifTrue: [kierunek := 6].
	(ruchy > 0)
		ifTrue: [kierunek := 8].
	(ruchy < 0)
		ifTrue: [kierunek := 2].
	(ody = doy) & (odx = dox)
		ifTrue: [kierunek := 5].
	
	(((ruchx abs) > 1) | ((ruchy abs) > 1))
		ifTrue: [w := self ktoryRycerz skocz: kierunek.]
		ifFalse: [w := self ktoryRycerz rusz: kierunek.]. 
		
	w isNil 
		ifTrue: ['blad w zmianie rycerzy.' displayOn: Transcript. Transcript flush.^self.].
	w
		ifFalse: [^self].
	
	(self tiktak)
		ifTrue: [self trigger: #zmianaR with: (self ktoryRycerz).]
		ifFalse: [self trigger: #zmianaR with: nil].
!

tiktak
	self aktualizujRycerzy.
	self czyKoniecGry
		ifTrue: [^false].
	self czyKoniecTury 
		ifFalse: [^true].
	plansza tiktak.
	self aktualizujRycerzy.
	self czyKoniecGry
		ifTrue: [^false].
	self trigger:  #aktualizacjaPlanszy.
	^true!

value
	^value.!

value: x
	value := x.
	self trigger: #zmianaOpisu!

wczytaj: nazwa
	|pl rozm x y wsp poczx poczy ryc ky kx pola str s stream plik pocz|

	"zresetowanie obecnych wartosci"
	self initialize.

	nazwa isNil
		ifTrue: [
			zapis isNil 
				ifTrue: [^false]
				ifFalse: [^self przypisz: zapis].
			].

	"wczytanie danych z pliku"
	(File exists: nazwa)
		ifFalse: [MessageBox notify: 'Bledna lokalizacja pliku'. ^false].
	
	plik := File open: nazwa.
	stream := plik readWriteStream.  s := stream size. pl := ''.
	[stream position < s] 
		whileTrue: [
			str := stream next. 
			pl := pl , (str asString)
		].
	plik close.
	pl := pl subStrings.
	zapis := pl.

	^self przypisz: pl.
	!

zmienNaRycerza: rycerz1 Z: rycerz2
	|k1 k2|
	((((rycerz1 pole) = (rycerz2 pole)) not) & ((rycerz1 tura) = (rycerz2 tura)))
		ifTrue: [
			rycerzeZywi keysAndValuesDo: [ :key :r | 
				(r = rycerz1) 
					ifTrue: [k1 := key].
				(r = rycerz2)
					ifTrue: [k2 := key].
			].
			rycerzeZywi
				removeKey: k1;
				removeKey: k2;
				at: k1 put: rycerz2;
				at: k2 put: rycerz1.
			self trigger: #zmianaR with: rycerz1
		]
		ifFalse: [(rycerz1 = rycerz2) ifFalse: [self value: ('Rycerz juz wykonal ruch w tej turze!!
' , self value)]].
	^self
	!

zmienNaRycerzaX: x Y: y Z: z
	|tmp|
	tmp := Array with: x with: y.
	rycerzeZywi do: [ :r | ((r pole wsp asArray) = tmp) ifTrue: [self zmienNaRycerza: r Z: z]]
	! !
!Gra categoriesFor: #aktualizujRycerzy!public! !
!Gra categoriesFor: #czyKoniecGry!public! !
!Gra categoriesFor: #czyKoniecTury!public! !
!Gra categoriesFor: #dodajR:pocz:!public! !
!Gra categoriesFor: #initialize!public! !
!Gra categoriesFor: #ktoryRycerz!public! !
!Gra categoriesFor: #opisX:Y:!public! !
!Gra categoriesFor: #pokaz!public! !
!Gra categoriesFor: #pominTure!public! !
!Gra categoriesFor: #przypisz:!public! !
!Gra categoriesFor: #ruszOd:od:do:do:!public! !
!Gra categoriesFor: #tiktak!public! !
!Gra categoriesFor: #value!public! !
!Gra categoriesFor: #value:!public! !
!Gra categoriesFor: #wczytaj:!public! !
!Gra categoriesFor: #zmienNaRycerza:Z:!public! !
!Gra categoriesFor: #zmienNaRycerzaX:Y:Z:!public! !

!Gra class methodsFor!

new
^(super new) initialize.! !
!Gra class categoriesFor: #new!public! !

Plansza guid: (GUID fromString: '{E69A2E30-4E0B-46E0-976F-56536A8C8CD7}')!
Plansza comment: ''!
!Plansza categoriesForClass!Unclassified! !
!Plansza methodsFor!

dodaj: pole
	pola at: ((pole wsp) asArray) put: pole.
!

initialize
	pola := Dictionary new.
	self dodaj: (Poza new: self).!

pola
	^pola!

pole: pole kierunek: kierunek
	^self sasiedzi pole at: kierunek ifAbsent: [self pole: #(0 0)]!

sasiedzi: pole
|wx wy wynik|
wx := pole wsp x.
wy := pole wsp y.
wynik := Dictionary new.
wynik at: 7 put: (pola at: (Array with: (wx - 1) with: (wy - 1)) ifAbsent: [pola at: #(0 0).]).
wynik at: 8 put: (pola at: (Array with: wx  with: (wy - 1)) ifAbsent: [pola at: #(0 0).]).
wynik at: 9 put: (pola at: (Array with: (wx + 1) with: (wy - 1)) ifAbsent: [pola at: #(0 0).]).
wynik at: 4 put: (pola at: (Array with: (wx - 1) with: wy) ifAbsent: [pola at: #(0 0).]).
wynik at: 6 put: (pola at: (Array with: (wx + 1) with: wy) ifAbsent: [pola at: #(0 0).]).
wynik at: 1 put: (pola at: (Array with: (wx - 1) with: (wy + 1)) ifAbsent: [pola at: #(0 0).]).
wynik at: 2 put: (pola at: (Array with: wx  with: (wy + 1)) ifAbsent: [pola at: #(0 0).]).
wynik at: 3 put: (pola at: (Array with: (wx + 1) with: (wy + 1)) ifAbsent: [pola at: #(0 0).]).
^wynik!

tiktak
	pola do: [ :p | p tiktak].!

wspX: xx Y: yy
	^pola at: (Array with: xx with: yy) ifAbsent: [pola at: #(0 0)]!

x
	^x!

x: xx
	x := xx!

y
	^y!

y: yy
	y := yy! !
!Plansza categoriesFor: #dodaj:!public! !
!Plansza categoriesFor: #initialize!public! !
!Plansza categoriesFor: #pola!public! !
!Plansza categoriesFor: #pole:kierunek:!public! !
!Plansza categoriesFor: #sasiedzi:!public! !
!Plansza categoriesFor: #tiktak!public! !
!Plansza categoriesFor: #wspX:Y:!public! !
!Plansza categoriesFor: #x!public! !
!Plansza categoriesFor: #x:!public! !
!Plansza categoriesFor: #y!public! !
!Plansza categoriesFor: #y:!public! !

!Plansza class methodsFor!

new
^super new initialize.! !
!Plansza class categoriesFor: #new!public! !

Pole guid: (GUID fromString: '{6A86D8D4-AB81-488D-89FF-6522EEFCABF7}')!
Pole comment: ''!
!Pole categoriesForClass!Unclassified! !
!Pole methodsFor!

czyJestKtos
	^((ktojest size) > 0)!

czyMoznaCegly
	^false!

czyMoznaTeleportowac
	^true.!

czyMoznaWejsc
	^(self czyJestKtos) not.!

czyMoznaZalatac
	^false!

czyMoznaZeby
	^false!

czyMoznaZjesc
	^false!

dodajR: rycerz
	self wejdz: rycerz.!

initialize: pl with: x with: y
	wsp := Wspolrzedne newX: x Y: y.
	ktojest := Set new.
	czywidoczne := false.
	plansza := pl.
	obrazki := 'C:\Documents and Settings\pt\Moje dokumenty\Dolphin Smalltalk 5.1\img\'.
	wygladniewid := Bitmap fromFile: (obrazki , 'niewid.bmp').
!

pokaz
	|ryc|
	czywidoczne
		ifFalse: [^wygladniewid].
	self czyJestKtos
		ifTrue: [ktojest do:  [ :r | ryc := r]. ^ryc pokaz.].
	^wygladwid.
!

sasiedzi
	^plansza sasiedzi: self.!

tiktak
	^self!

uwidocznijSasiadow
	(self sasiedzi) do: [ :x | x widoczne.].!

wejdz: rycerz
	ktojest add: rycerz.
	self widoczne.
	self uwidocznijSasiadow.
	^true!

widoczne
	czywidoczne := true!

wsp
	^wsp!

wybuch
	ktojest do: [ :x | x smierc.].!

wyjdz: rycerz
ktojest remove: rycerz ifAbsent: [^self]!

wypisz
	|ryc|
	czywidoczne
		ifTrue: [^'Pole zwykle
' , (self wypiszRycerzy).]
		ifFalse: [^'Brak danych'].!

wypiszRycerzy
	|tmp|
	tmp := ''.
	ktojest do: [ :r | tmp := tmp , (r wypisz)].
	^tmp! !
!Pole categoriesFor: #czyJestKtos!public! !
!Pole categoriesFor: #czyMoznaCegly!public! !
!Pole categoriesFor: #czyMoznaTeleportowac!public! !
!Pole categoriesFor: #czyMoznaWejsc!public! !
!Pole categoriesFor: #czyMoznaZalatac!public! !
!Pole categoriesFor: #czyMoznaZeby!public! !
!Pole categoriesFor: #czyMoznaZjesc!public! !
!Pole categoriesFor: #dodajR:!public! !
!Pole categoriesFor: #initialize:with:with:!public! !
!Pole categoriesFor: #pokaz!public! !
!Pole categoriesFor: #sasiedzi!public! !
!Pole categoriesFor: #tiktak!public! !
!Pole categoriesFor: #uwidocznijSasiadow!public! !
!Pole categoriesFor: #wejdz:!public! !
!Pole categoriesFor: #widoczne!public! !
!Pole categoriesFor: #wsp!public! !
!Pole categoriesFor: #wybuch!public! !
!Pole categoriesFor: #wyjdz:!public! !
!Pole categoriesFor: #wypisz!public! !
!Pole categoriesFor: #wypiszRycerzy!public! !

!Pole class methodsFor!

newPl: pl X: x Y: y
^(super new) initialize: pl with: x with: y.! !
!Pole class categoriesFor: #newPl:X:Y:!public! !

Rycerz guid: (GUID fromString: '{9BAA0754-1A64-4EFA-B0B3-FBB7CEA299B0}')!
Rycerz comment: ''!
!Rycerz categoriesForClass!Unclassified! !
!Rycerz methodsFor!

chceszCegly
	^self!

chceszZeby
	^self!

initialize: p
	obrazki := 'C:\Documents and Settings\pt\Moje dokumenty\Dolphin Smalltalk 5.1\img\'.
	pole := p.
	tura := 0.
	stan := 1.!

ok
	stan := 2!

pokaz
	^wyglad!

pole
	^pole!

rusz: kierunek
	|cel|
	cel := ((pole sasiedzi) at: kierunek ifAbsent: [tura := tura + 1.^true]).
	^self ruszNaPole: cel.!

ruszNaPole: cel
	cel czyMoznaWejsc
		ifFalse: [^false].

	pole wyjdz: self. 
	pole := cel.
	cel wejdz: self.
	tura := tura + 1.
	^true.
!

skocz: kierunek
	^false!

smierc
	pole wyjdz: self.
	stan := 0
	!

stan
	^stan!

stan: x
	stan := x.!

tura
	^tura!

tura: n
	tura := n.!

wypisz
	^'nigdy nie powinno sie to wyswietlic'! !
!Rycerz categoriesFor: #chceszCegly!public! !
!Rycerz categoriesFor: #chceszZeby!public! !
!Rycerz categoriesFor: #initialize:!public! !
!Rycerz categoriesFor: #ok!public! !
!Rycerz categoriesFor: #pokaz!public! !
!Rycerz categoriesFor: #pole!public! !
!Rycerz categoriesFor: #rusz:!public! !
!Rycerz categoriesFor: #ruszNaPole:!public! !
!Rycerz categoriesFor: #skocz:!public! !
!Rycerz categoriesFor: #smierc!public! !
!Rycerz categoriesFor: #stan!public! !
!Rycerz categoriesFor: #stan:!public! !
!Rycerz categoriesFor: #tura!public! !
!Rycerz categoriesFor: #tura:!public! !
!Rycerz categoriesFor: #wypisz!public! !

!Rycerz class methodsFor!

new: pole
^(self new) initialize: pole! !
!Rycerz class categoriesFor: #new:!public! !

Wspolrzedne guid: (GUID fromString: '{0938EA01-A3E7-4A66-9292-7C2D337D7C77}')!
Wspolrzedne comment: ''!
!Wspolrzedne categoriesForClass!Unclassified! !
!Wspolrzedne methodsFor!

asArray
	^Array with: x with: y.!

initialize: wx and: wy
	x := wx.
	y := wy.!

x
	^x!

y
	^y! !
!Wspolrzedne categoriesFor: #asArray!public! !
!Wspolrzedne categoriesFor: #initialize:and:!public! !
!Wspolrzedne categoriesFor: #x!public! !
!Wspolrzedne categoriesFor: #y!public! !

!Wspolrzedne class methodsFor!

newX: x Y: y
	^(super new) initialize: x and: y.! !
!Wspolrzedne class categoriesFor: #newX:Y:!public! !

Legowisko guid: (GUID fromString: '{5E9EE37B-3B59-405A-B54C-8B54AF3AD6F9}')!
Legowisko comment: ''!
!Legowisko categoriesForClass!Unclassified! !
!Legowisko methodsFor!

czyMoznaWejsc
	^true!

initialize
	wygladwid := Bitmap fromFile: (obrazki , 'legowisko.bmp').!

pokaz
	czywidoczne
		ifTrue: [^wygladwid]
		ifFalse: [^wygladniewid].!

wejdz: rycerz
	super wejdz: rycerz.
	rycerz ok.!

wybuch
	^self!

wypisz
	czywidoczne
		ifTrue: [^'Legowisko' , '
Rycerzy: ' , (ktojest size printString)]
		ifFalse: [^'Brak danych'].! !
!Legowisko categoriesFor: #czyMoznaWejsc!public! !
!Legowisko categoriesFor: #initialize!public! !
!Legowisko categoriesFor: #pokaz!public! !
!Legowisko categoriesFor: #wejdz:!public! !
!Legowisko categoriesFor: #wybuch!public! !
!Legowisko categoriesFor: #wypisz!public! !

!Legowisko class methodsFor!

newPl: pl X: x Y: y
^(super newPl: pl X: x Y: y) initialize.! !
!Legowisko class categoriesFor: #newPl:X:Y:!public! !

Magazyn guid: (GUID fromString: '{D607333A-EAF7-4CA1-983E-4364741E67EF}')!
Magazyn comment: ''!
!Magazyn categoriesForClass!Unclassified! !
!Magazyn methodsFor!

initialize: z
	czasreaktywacji := 50.
	licznik := 0.
	pojemnosc := z.
	ilejest := z.!

pobierz: n
	|tmp|
	tmp := ilejest.
	licznik = 0
		ifTrue: [licznik := czasreaktywacji].

	n > ilejest
		ifTrue: [ilejest := 0.^tmp]
		ifFalse: [ilejest := ilejest - n.^n].
!

tiktak
	licznik = 1
		ifTrue: [ilejest := ilejest + 1].
	
	ilejest = pojemnosc
		ifFalse: [licznik = 0
			ifTrue: [licznik := czasreaktywacji]
			ifFalse: [licznik := licznik - 1].]
		ifTrue: [licznik := 0.].
	ilejest > 0
		ifTrue: [self jestEkwipunek]! !
!Magazyn categoriesFor: #initialize:!public! !
!Magazyn categoriesFor: #pobierz:!public! !
!Magazyn categoriesFor: #tiktak!public! !

!Magazyn class methodsFor!

newPl: pl X: x Y: y poj: z
^(super newPl: pl X: x Y: y) initialize: z.! !
!Magazyn class categoriesFor: #newPl:X:Y:poj:!public! !

Mur guid: (GUID fromString: '{F73F7DD6-DE56-4FD6-97BF-C1ACD7D95E7F}')!
Mur comment: ''!
!Mur categoriesForClass!Unclassified! !
!Mur methodsFor!

czyMoznaTeleportowac
	^false!

czyMoznaWejsc
	^false! !
!Mur categoriesFor: #czyMoznaTeleportowac!public! !
!Mur categoriesFor: #czyMoznaWejsc!public! !

!Mur class methodsFor!

newPl: pl X: x Y: y
^(super newPl: pl X: x Y: y) initialize.! !
!Mur class categoriesFor: #newPl:X:Y:!public! !

Poza guid: (GUID fromString: '{0C39F754-1ED5-4565-8AF5-F8B1C5FC0BC3}')!
Poza comment: ''!
!Poza categoriesForClass!Unclassified! !
!Poza methodsFor!

czyMoznaTeleportowac
	^false!

czyMoznaWejsc
	^false!

initialize
	^self!

pokaz
^self!

widoczne
^self!

wybuch
^self! !
!Poza categoriesFor: #czyMoznaTeleportowac!public! !
!Poza categoriesFor: #czyMoznaWejsc!public! !
!Poza categoriesFor: #initialize!public! !
!Poza categoriesFor: #pokaz!public! !
!Poza categoriesFor: #widoczne!public! !
!Poza categoriesFor: #wybuch!public! !

!Poza class methodsFor!

new: pl
istniejacyobiekt = nil
	ifTrue: [^(self newPl: pl X: 0 Y: 0) initialize]
	ifFalse: [^istniejacyobiekt].! !
!Poza class categoriesFor: #new:!public! !

Pulapka guid: (GUID fromString: '{ABE2F9BD-91FB-4CCD-9EF9-66C68FB195C3}')!
Pulapka comment: ''!
!Pulapka categoriesForClass!Unclassified! !
!Pulapka methodsFor!

initialize
	licznik := 0.!

pokaz
	|ryc|
	czywidoczne
		ifFalse: [^wygladniewid].
	licznik = 0
		ifTrue: [^wygladwid].
	self czyJestKtos
		ifTrue: [ktojest do: [ :r | ryc := r]. ^ryc pokaz.]
		ifFalse: [^wygladnieakt].!

tiktak
	licznik > 0
		ifTrue: [licznik := licznik - 1.].
	licznik = 0
		ifTrue: [ktojest do: [ :x | x smierc]].!

wejdz: rycerz
	licznik > 0
		ifTrue: [super wejdz: rycerz.^true].
	rycerz smierc.
	^true!

zabij
"narazie nie implementuje"! !
!Pulapka categoriesFor: #initialize!public! !
!Pulapka categoriesFor: #pokaz!public! !
!Pulapka categoriesFor: #tiktak!public! !
!Pulapka categoriesFor: #wejdz:!public! !
!Pulapka categoriesFor: #zabij!public! !

!Pulapka class methodsFor!

newPl: pl X: x Y: y
^(super newPl: pl X: x Y: y) initialize.! !
!Pulapka class categoriesFor: #newPl:X:Y:!public! !

Teleport guid: (GUID fromString: '{CC259591-7961-4270-A50D-C1B4ED2FDB5D}')!
Teleport comment: ''!
!Teleport categoriesForClass!Unclassified! !
!Teleport methodsFor!

czyMoznaTeleportowac
	^false!

initialize: d
dokad := d.
wygladwid := Bitmap fromFile: (obrazki , 'teleport.bmp').!

teleportuj: rycerz
	|p|
	p := plansza wspX: (dokad at: 1) Y: (dokad at: 2).
	p czyMoznaTeleportowac
		ifTrue: [(p czyMoznaWejsc) ifTrue: [rycerz tura: ((rycerz tura) - 1). ^rycerz ruszNaPole: p]].
	^false!

tiktak
	ktojest do: [:r | self teleportuj: r].!

wejdz: rycerz
	(self teleportuj: rycerz)
		ifTrue: [^true].
	super wejdz: rycerz.!

wypisz
	czywidoczne
		ifTrue: [
^'Teleport' , '
prowadzi do: ' , '
x = ' , ((dokad at: 1) printString) , '
y = ' , ((dokad at: 2) printString) , (self wypiszRycerzy)
		]
		ifFalse: [^'Brak danych'].! !
!Teleport categoriesFor: #czyMoznaTeleportowac!public! !
!Teleport categoriesFor: #initialize:!public! !
!Teleport categoriesFor: #teleportuj:!public! !
!Teleport categoriesFor: #tiktak!public! !
!Teleport categoriesFor: #wejdz:!public! !
!Teleport categoriesFor: #wypisz!public! !

!Teleport class methodsFor!

newPl: pl X: x Y: y dokad: z
^(super newPl: pl X: x Y: y) initialize: z.! !
!Teleport class categoriesFor: #newPl:X:Y:dokad:!public! !

Zwykle guid: (GUID fromString: '{FF0AF1D2-2AF6-4721-801C-4E8EBCA21F9C}')!
Zwykle comment: ''!
!Zwykle categoriesForClass!Unclassified! !
!Zwykle methodsFor!

initialize
wygladwid := Bitmap fromFile: (obrazki , 'zwykle.bmp').! !
!Zwykle categoriesFor: #initialize!public! !

!Zwykle class methodsFor!

newPl: pl X: x Y: y
^(super newPl: pl X: x Y: y) initialize.! !
!Zwykle class categoriesFor: #newPl:X:Y:!public! !

Budowlany guid: (GUID fromString: '{7E4F7252-45D0-4548-8191-FD16D2BF81BF}')!
Budowlany comment: ''!
!Budowlany categoriesForClass!Unclassified! !
!Budowlany methodsFor!

czyMoznaCegly
	^ilejest > 0!

initialize: z
	super initialize: z.
	wygladwid := Bitmap fromFile: (obrazki , 'budowlany.bmp')!

jestEkwipunek
	ktojest do: [ :r | r chceszCegly].!

wypisz
	czywidoczne
		ifTrue: [
			^('Magazyn
budowlany
' , 'liczba cegiel: ' , (ilejest printString) , '
pojemnoœæ: ' , (pojemnosc printString) , '
licznik: ' , (licznik printString) , (self wypiszRycerzy))
		]
		ifFalse: [^'Brak danych'].! !
!Budowlany categoriesFor: #czyMoznaCegly!public! !
!Budowlany categoriesFor: #initialize:!public! !
!Budowlany categoriesFor: #jestEkwipunek!public! !
!Budowlany categoriesFor: #wypisz!public! !

Dentystyczny guid: (GUID fromString: '{3700689D-E425-447E-9353-A4766BB36782}')!
Dentystyczny comment: ''!
!Dentystyczny categoriesForClass!Unclassified! !
!Dentystyczny methodsFor!

czyMoznaZeby
	^ilejest > 0!

initialize: z
	super initialize: z.
	wygladwid := Bitmap fromFile: (obrazki , 'dentystyczny.bmp').!

jestEkwipunek
	ktojest do: [ :r | r chceszZeby].!

wypisz
	czywidoczne
		ifTrue: [
			^('Magazyn
dentystyczny
liczba zebow: ' , (ilejest printString) ,'
pojemnosc: ' , (pojemnosc printString) ,'
licznik: ' , (licznik printString) , (self wypiszRycerzy))
		]
		ifFalse: [^'Brak danych'].! !
!Dentystyczny categoriesFor: #czyMoznaZeby!public! !
!Dentystyczny categoriesFor: #initialize:!public! !
!Dentystyczny categoriesFor: #jestEkwipunek!public! !
!Dentystyczny categoriesFor: #wypisz!public! !

Wieczny guid: (GUID fromString: '{E4359007-990C-4BC8-95A8-001F4AA26495}')!
Wieczny comment: ''!
!Wieczny categoriesForClass!Unclassified! !
!Wieczny methodsFor!

initialize
wygladwid := Bitmap fromFile: (obrazki , 'murwieczny.bmp').!

wybuch
	^self!

wypisz
	czywidoczne
		ifTrue: [^'Mur wieczny']
		ifFalse: [^'Brak danych'].! !
!Wieczny categoriesFor: #initialize!public! !
!Wieczny categoriesFor: #wybuch!public! !
!Wieczny categoriesFor: #wypisz!public! !

Zjadalny guid: (GUID fromString: '{B6B2DD89-F7BA-43BD-8082-D09B8B30B1EF}')!
Zjadalny comment: ''!
!Zjadalny categoriesForClass!Unclassified! !
!Zjadalny methodsFor!

czyMoznaTeleportowac
	licznik > 0
		ifTrue: [super czyMoznaTeleportowac].
	^false!

czyMoznaWejsc
	^((licznik > 0) & (self czyJestKtos not))!

czyMoznaZjesc
	^licznik = 0!

initialize
	licznik := 0.
	wygladwid := Bitmap fromFile: (obrazki , 'murzjadalny.bmp').
	wygladruiny := Bitmap fromFile: (obrazki , 'murzjadalnyruina.bmp').
	czasreaktywacji := 20.!

pokaz
	|ryc|
	czywidoczne
		ifFalse: [^wygladniewid].
	licznik = 0
		ifTrue: [^wygladwid].
	self czyJestKtos
		ifTrue: [ktojest do: [ :r | ryc := r]. ^ryc pokaz]
		ifFalse: [^wygladruiny].!

tiktak
	licznik > 0
		ifTrue: [licznik := licznik - 1].
	licznik = 0
		ifTrue: [ktojest do: [ :x | x smierc]].!

wybuch
	super wybuch.
	licznik := czasreaktywacji.!

wypisz
	czywidoczne
		ifTrue: [
^'Mur zjadalny' , '
licznik: ' , (licznik printString) , (self wypiszRycerzy)
		]
		ifFalse: [^'Brak danych'].!

zjedz
	licznik := czasreaktywacji.
	^true!

zjedz: rycerz
licznik := czasreaktywacji.
ktojest add: rycerz.
^true! !
!Zjadalny categoriesFor: #czyMoznaTeleportowac!public! !
!Zjadalny categoriesFor: #czyMoznaWejsc!public! !
!Zjadalny categoriesFor: #czyMoznaZjesc!public! !
!Zjadalny categoriesFor: #initialize!public! !
!Zjadalny categoriesFor: #pokaz!public! !
!Zjadalny categoriesFor: #tiktak!public! !
!Zjadalny categoriesFor: #wybuch!public! !
!Zjadalny categoriesFor: #wypisz!public! !
!Zjadalny categoriesFor: #zjedz!public! !
!Zjadalny categoriesFor: #zjedz:!public! !

Dziura guid: (GUID fromString: '{A2F3B5D6-399E-4C13-846A-518E7B2D749A}')!
Dziura comment: ''!
!Dziura categoriesForClass!Unclassified! !
!Dziura methodsFor!

czyMoznaZalatac
^licznik = 0!

initialize: szer
szerokosc := szer.
wygladwid := Bitmap fromFile: (obrazki , 'dziura.bmp').
wygladnieakt := Bitmap fromFile: (obrazki , 'dziuranieakt.bmp').
czasreaktywacji := 30.!

szerokosc
	^szerokosc!

wybuch
	licznik := 0.
	ktojest do: [ :x | x smierc.]!

wypisz
	czywidoczne
		ifTrue: [
^'Dziura' , '
szerokosc: ' , (szerokosc printString) , '
licznik: ' , (licznik printString) , (self wypiszRycerzy)
		]
		ifFalse: [^'Brak danych'].!

zalataj: n
	szerokosc > n
		ifTrue: [^n].
	licznik := czasreaktywacji.
	^n - szerokosc.! !
!Dziura categoriesFor: #czyMoznaZalatac!public! !
!Dziura categoriesFor: #initialize:!public! !
!Dziura categoriesFor: #szerokosc!public! !
!Dziura categoriesFor: #wybuch!public! !
!Dziura categoriesFor: #wypisz!public! !
!Dziura categoriesFor: #zalataj:!public! !

!Dziura class methodsFor!

newPl: pl X: x Y: y szerokosc: szer
^(super newPl: pl X: x Y: y) initialize: szer! !
!Dziura class categoriesFor: #newPl:X:Y:szerokosc:!public! !

Mina guid: (GUID fromString: '{D6A56952-288D-4BBC-B589-7544EC4D1328}')!
Mina comment: ''!
!Mina categoriesForClass!Unclassified! !
!Mina methodsFor!

initialize
super initialize.
wygladwid := Bitmap fromFile: (obrazki , 'mina.bmp').
wygladnieakt := Bitmap fromFile: (obrazki , 'minanieakt.bmp').
czasreaktywacji := 20.!

tiktak
	((self czyJestKtos) & (licznik = 1))
		ifTrue: [licznik := 0.self wybuch.^self].

	super tiktak.
	!

wejdz: rycerz
	licznik > 0
		ifTrue: [super wejdz: rycerz.^true].
	rycerz smierc.
	self wybuch.
	^true!

wybuch
	ktojest do: [ :x | x smierc].
	licznik = 0
		ifTrue: [licznik := czasreaktywacji. (self sasiedzi) do: [ :x | x wybuch]].
	
!

wypisz
	czywidoczne
		ifTrue: [
^'Mina' , '
licznik: ' , (licznik printString) , (self wypiszRycerzy)
		]
		ifFalse: [^'Brak danych'].! !
!Mina categoriesFor: #initialize!public! !
!Mina categoriesFor: #tiktak!public! !
!Mina categoriesFor: #wejdz:!public! !
!Mina categoriesFor: #wybuch!public! !
!Mina categoriesFor: #wypisz!public! !

!Mina class methodsFor!

newPl: pl X: x Y: y 
^(super newPl: pl X: x Y: y) initialize! !
!Mina class categoriesFor: #newPl:X:Y:!public! !

GraMapaPresenter guid: (GUID fromString: '{6A98E21C-7661-40D9-AF23-958D10FF757F}')!
GraMapaPresenter comment: ''!
!GraMapaPresenter categoriesForClass!Unclassified! !
!GraMapaPresenter methodsFor!

createComponents
	super createComponents.
!

onLeftButtonPressed: mouse
	view mouseLeftX: (mouse x) Y: (mouse y).!

onRightButtonPressed: mouse
	view mouseRightX: (mouse x) Y: (mouse y).!

wczytajPl: pl r: r  wys: wys szer: szer
	view wczytajPl: pl r: r wys: wys szer: szer
! !
!GraMapaPresenter categoriesFor: #createComponents!public! !
!GraMapaPresenter categoriesFor: #onLeftButtonPressed:!public! !
!GraMapaPresenter categoriesFor: #onRightButtonPressed:!public! !
!GraMapaPresenter categoriesFor: #wczytajPl:r:wys:szer:!public! !

!GraMapaPresenter class methodsFor!

defaultView
	^'GraMapaView'! !
!GraMapaPresenter class categoriesFor: #defaultView!public! !

GraPresenter guid: (GUID fromString: '{A040F367-6402-4D4B-BAD7-47B5BF2A28AD}')!
GraPresenter comment: ''!
!GraPresenter categoriesForClass!Unclassified! !
!GraPresenter methodsFor!

createComponents
	super createComponents.

	self add: (TextPresenter new) name: 'txt'. 
	self add: (GraMapaPresenter new) name: 'Mapa'.!

koniecGry: a
	a = 0
		ifTrue: [
			MessageBox notify: 'Niestety przegrales !!'.
		]
		ifFalse: [
			MessageBox notify: 'Gratulacje wygrales !!'.
		].!

model: aModel
	super model: aModel.
	
	(self presenterNamed: 'txt') model: (aModel aspectValue: #value).
	(self presenterNamed: 'Mapa') model: aModel.
	
	aModel when: #zmianaOpisu send: #opis to: self.
	aModel when: #koniecGry send: #koniecGry: to: self.
!

nowaGra
	self wczytaj: nil.!

opis
	(self presenterNamed: 'txt') view refreshContents.!

pominTure
	model pominTure.!

pomoc
	|tresc|
	tresc := '(1) Przyciski:
Przycisk "Wczytaj gre":
	wczytuje gre z podanego pliku;
Przycisk "Nowa gra":
	rozpoczyna od nowa ostatnio wczytana gre;
Przycisk "Pomin ture":
	pomija ruch kazdego rycerza w turze.

(2) Plansza:
Lewy przycik myszy:
	Zmiana aktywnego rycerza i/lub jest
	wyswietlany jest opis pola w okienku statusu gry
Prawy przycisk myszy:
	Wykonywany jest ruch aktywnym* rycerzem (o ile jest dozwolony).

(3) Gra:
Rycerze maja za cel dojscie do legowiska smoka.

---
(*) Rycerz aktywny jest na polu oznaczonym czarnym obramowaniem.'.
	MessageBox notify: tresc.!

wczytaj: plik
	|pl r wys szer|

	(model wczytaj: plik)
		ifFalse: [^self].
	pl := model pokaz.
	r := model ktoryRycerz.
	wys := pl y.
	szer := pl x.

	(self presenterNamed: 'Mapa') wczytajPl: pl r: r wys: wys szer: szer.
!

wczytajGre
	|wczytano|
	wczytano := Prompter prompt: 'Podaj lokalizacje pliku...'.
	wczytano isNil
		ifTrue: [^self].
	self wczytaj: wczytano.

! !
!GraPresenter categoriesFor: #createComponents!public! !
!GraPresenter categoriesFor: #koniecGry:!public! !
!GraPresenter categoriesFor: #model:!public! !
!GraPresenter categoriesFor: #nowaGra!public! !
!GraPresenter categoriesFor: #opis!public! !
!GraPresenter categoriesFor: #pominTure!public! !
!GraPresenter categoriesFor: #pomoc!public! !
!GraPresenter categoriesFor: #wczytaj:!public! !
!GraPresenter categoriesFor: #wczytajGre!public! !

!GraPresenter class methodsFor!

defaultModel
^Gra new.!

defaultView
^'GraView'! !
!GraPresenter class categoriesFor: #defaultModel!public! !
!GraPresenter class categoriesFor: #defaultView!public! !

Nadzwyczajny guid: (GUID fromString: '{6918594A-8F0B-47A9-A792-BB1918C615ED}')!
Nadzwyczajny comment: ''!
!Nadzwyczajny categoriesForClass!Unclassified! !
!Nadzwyczajny methodsFor!

initialize: p
	super initialize: p.
	wyglad := Bitmap fromFile: (obrazki , 'nadzwyczajny.bmp').
	cecha := 'niewiarygodny upór'.!

wypisz
	^'
--------------------------------
Ryczerz Nadzwyczajny'! !
!Nadzwyczajny categoriesFor: #initialize:!public! !
!Nadzwyczajny categoriesFor: #wypisz!public! !

Skoczek guid: (GUID fromString: '{294BDC4A-B38E-4B9F-B0CF-0645371D1B59}')!
Skoczek comment: ''!
!Skoczek categoriesForClass!Unclassified! !
!Skoczek methodsFor!

initialize: p
super initialize: p.
wyglad := Bitmap fromFile: (obrazki , 'skoczek.bmp').!

skocz: kierunek
	|cel|
	cel := ((pole sasiedzi at: kierunek) sasiedzi at: kierunek).
	^self ruszNaPole: cel.!

wypisz
^'
--------------------------------
Rycerz Skoczek'! !
!Skoczek categoriesFor: #initialize:!public! !
!Skoczek categoriesFor: #skocz:!public! !
!Skoczek categoriesFor: #wypisz!public! !

Wyposazony guid: (GUID fromString: '{466B7405-2B44-48B2-BBF2-9681B1C62584}')!
Wyposazony comment: ''!
!Wyposazony categoriesForClass!Unclassified! !
!Wyposazony methodsFor!

initializei: i
	iloscekw := i.
	maxekw := i.!

wez: cel
	|tmp|
	tmp := maxekw - iloscekw.
	tmp > 0
		ifTrue: [iloscekw := (cel pobierz: tmp) + iloscekw.].! !
!Wyposazony categoriesFor: #initializei:!public! !
!Wyposazony categoriesFor: #wez:!public! !

!Wyposazony class methodsFor!

new: p il: i
^(self new: p) initializei: i! !
!Wyposazony class categoriesFor: #new:il:!public! !

Budowniczy guid: (GUID fromString: '{2FC81A98-D530-4987-BFFC-A548AB84C6FF}')!
Budowniczy comment: ''!
!Budowniczy categoriesForClass!Unclassified! !
!Budowniczy methodsFor!

chceszCegly
	self wez: pole.!

initializei: i
super initializei: i.
wyglad := Bitmap fromFile: (obrazki , 'budowniczy.bmp')!

rusz: kierunek
	|cel|
	cel := ((pole sasiedzi) at: kierunek ifAbsent: [tura := tura + 1.^true]).

	cel czyMoznaZalatac
		ifTrue: [self zalataj: cel.].

	((cel czyMoznaWejsc) & (cel czyMoznaCegly))
		ifTrue: [self wez: cel.].

	^self ruszNaPole: cel.!

wypisz
^'
--------------------------------
Rycerz Budowniczy' , '
ilosc cegiel: ' , (iloscekw printString) , '
pojemnosc worka: ' , (maxekw printString) 
!

zalataj: cel
	|pozostalo|
	pozostalo := cel zalataj: iloscekw.
	
	pozostalo = iloscekw
		ifTrue: [^false].
	
	iloscekw := pozostalo.
	^true.

! !
!Budowniczy categoriesFor: #chceszCegly!public! !
!Budowniczy categoriesFor: #initializei:!public! !
!Budowniczy categoriesFor: #rusz:!public! !
!Budowniczy categoriesFor: #wypisz!public! !
!Budowniczy categoriesFor: #zalataj:!public! !

Zjadacz guid: (GUID fromString: '{C8CAD3DC-97CC-47CF-B0B5-F1748B05F84D}')!
Zjadacz comment: ''!
!Zjadacz categoriesForClass!Unclassified! !
!Zjadacz methodsFor!

chceszZeby
	self wez: pole.!

initializei: i
	super initializei: i.
	wyglad := Bitmap fromFile: (obrazki , 'zjadacz.bmp').!

rusz: kierunek
	|cel|
	cel := ((pole sasiedzi) at: kierunek ifAbsent: [tura := tura + 1.^true]).

	cel czyMoznaZjesc
		ifTrue: [self zjedz: cel.].
	
	((cel czyMoznaWejsc) & (cel czyMoznaZeby))
		ifTrue: [self wez: cel.].

	^self ruszNaPole: cel.
!

wypisz
	^'
--------------------------------
Rycerz Zjadacz' , '
ilosc zebow: ' , (iloscekw printString) , '
pojemnosc szczeki: ' , (maxekw printString) 
!

zjedz: cel
	iloscekw > 0
		ifFalse: [^false].
	
	cel zjedz.
	iloscekw := iloscekw - 1.
	^true.

! !
!Zjadacz categoriesFor: #chceszZeby!public! !
!Zjadacz categoriesFor: #initializei:!public! !
!Zjadacz categoriesFor: #rusz:!public! !
!Zjadacz categoriesFor: #wypisz!public! !
!Zjadacz categoriesFor: #zjedz:!public! !

GraMapaView guid: (GUID fromString: '{ACA7DC0D-4F50-42B4-BA4E-3EE289C8C483}')!
GraMapaView comment: ''!
!GraMapaView categoriesForClass!Unclassified! !
!GraMapaView methodsFor!

initialize
	super initialize.
	rozmiar := 32.
	wysokosc := 0.
	szerokosc := 0.
!

koniec: x
"skoro koniec to zadnym rycerzem juz sie nie ruszamy"
	rycerz := nil.
	!

model: graModel
	super model: graModel.

	graModel when: #zmianaR send: #rycerz: to: self.
	graModel when: #aktualizacjaPlanszy send: #odswierz to: self.
	graModel when: #koniecGry send: #koniec: to: self!

mouseLeftX: x Y: y
"Wy?wietlanie opisu pola wraz z ewentualnymi rycerzami."
	|pozx pozy recx recy paint|

	rozmiar isNil 
		ifTrue: [rozmiar := 32. wysokosc := 0. szerokosc := 0.].
	pozx := (x // rozmiar) + 1.
	pozy := (y // rozmiar) + 1.

	((pozx < 1) | (pozy < 1) | (pozx > szerokosc) | (pozy > wysokosc))
		ifTrue: [pozx := 0. pozy := 0.^self].

	model opisX: pozx Y: pozy.
	opisX :=pozx.
	opisY := pozy.
	rycerz isNil
		ifTrue: [^self].
	model zmienNaRycerzaX: pozx Y: pozy Z: rycerz.
!

mouseRightX: x Y: y
"Wykonanie ruchu dla rycerza na wskazane pole"
	|pozx pozy imx imy|
	
	rozmiar isNil 
		ifTrue: [rozmiar := 32. wysokosc := 0. szerokosc := 0.].	
	pozx := (x // rozmiar) + 1. 
	pozy := (y // rozmiar) + 1. 

	((pozx < 1) | (pozy < 1) | (pozx > szerokosc) | (pozy > wysokosc))
		ifTrue: [pozx := 0. pozy := 0.^self].

	rycerz isNil
		ifTrue: [^self].
	
	model ruszOd: (rycerz pole wsp asArray at: 1 ) od: (rycerz pole wsp asArray at: 2) do: pozx do: pozy.

	self rysuj
	model opisX: opisX Y: opisY.
	"(self presenterNamed: 'txt') view refreshContents."!

odswierz
	self rysuj.
	model opisX: opisX Y: opisY.!

onPaintRequired: aPaintEvent
	self rysuj.!

onPositionChanged: aPositionEvent
	super onPositionChanged: aPositionEvent.
	
	rozmiar isNil ifTrue: [rozmiar := 32. wysokosc := 0. szerokosc := 0.].
	((wysokosc > 0) & (szerokosc > 0))
		ifTrue: [self rysuj].
	!

rycerz: r
	rycerz := r.
	self rysuj.!

rysuj
	|liczniky licznikx px py|
	
	rozmiar isNil ifTrue: [rozmiar := 32. wysokosc := 0. szerokosc := 0.].
	liczniky := 1.
	[liczniky <= wysokosc] whileTrue: [
		licznikx := 1.
		[licznikx <= szerokosc] whileTrue: [
			px := (licznikx - 1) * rozmiar.
			py := (liczniky - 1) * rozmiar.
			((plansza wspX: licznikx Y: liczniky) pokaz) drawOn: (self canvas) at: px@py extent: rozmiar@rozmiar.
			licznikx := licznikx + 1.
		].
		liczniky := liczniky + 1.
	].
" + zaznaczenie aktywnego rycerza"
	rycerz isNil 
		ifTrue: [^self].
	px := ((rycerz pole wsp asArray at: 1) - 1) * rozmiar. 
	py := ((rycerz pole wsp asArray at: 2) - 1) * rozmiar.
	(self canvas) 
		brush: Brush black;
		rectangle: (Rectangle origin: px@py extent: rozmiar@rozmiar).
	(rycerz pokaz) drawOn: (self canvas) at: (px + 1)@(py + 1) extent: (rozmiar - 2)@(rozmiar - 2).!

wczytajPl: pl r: r wys: wys szer: szer
	
	plansza := pl.
	rycerz := r.
	wysokosc := wys.
	szerokosc := szer.
	rozmiar := 32.
	opisX := 0.
	opisY := 0.
	
	self rysuj.! !
!GraMapaView categoriesFor: #initialize!public! !
!GraMapaView categoriesFor: #koniec:!public! !
!GraMapaView categoriesFor: #model:!public! !
!GraMapaView categoriesFor: #mouseLeftX:Y:!public! !
!GraMapaView categoriesFor: #mouseRightX:Y:!public! !
!GraMapaView categoriesFor: #odswierz!public! !
!GraMapaView categoriesFor: #onPaintRequired:!public! !
!GraMapaView categoriesFor: #onPositionChanged:!public! !
!GraMapaView categoriesFor: #rycerz:!public! !
!GraMapaView categoriesFor: #rysuj!public! !
!GraMapaView categoriesFor: #wczytajPl:r:wys:szer:!public! !

!GraMapaView class methodsFor!

new
	^(super new) initialize.! !
!GraMapaView class categoriesFor: #new!public! !

"Binary Globals"!

"Resources"!

(ResourceIdentifier class: GraMapaPresenter name: 'GraMapaView') assign: (Object fromBinaryStoreBytes:
(ByteArray fromHexString: '2153544220312046020C0001000000566965775265736F75726365000000000E0124005354425265736F757263655354424279746541727261794163636573736F7250726F78790000000072000000B7010000215354422031204E080C000A0000005354425669657750726F7879000000009A00000000000000520000000700000050726F6A656B74520000000B0000004772614D6170615669657762000000130000000000000000000000620000000200000082000000040000000000004401000000A001000000000000000000000000000005000000000000000000000000000000A00100000100000001000000000000000000000000000000000000004100000006010F004D65737361676553657175656E636500000000CA00000000000000D0000000620000000100000006030B004D65737361676553656E6400000000BA00000000000000520000001000000063726561746541743A657874656E743A620000000200000006020500506F696E74000000000100000001000000A202000000000000C9000000C9000000A001000006010F0057494E444F57504C4143454D454E5400000000720000002C0000002C0000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000000006400000064000000CA00000000000000D00000006200000000000000A202000000000000C1000000C10000000000000013000000460504000300000049636F6E0000000000000000100000000E02110053544253696E676C65746F6E50726F7879000000009A000000000000005200000007000000446F6C7068696E5200000018000000496D61676552656C617469766546696C654C6F6361746F72BA00000000000000520000000700000063757272656E745200000008000000566965772E69636F0E021F0053544245787465726E616C5265736F757263654C69627261727950726F7879000000005200000010000000646F6C7068696E64723030352E646C6C00000000'))!

(ResourceIdentifier class: GraPresenter name: 'GraView') assign: (Object fromBinaryStoreBytes:
(ByteArray fromHexString: '2153544220312046020C0001000000566965775265736F75726365000000000E0124005354425265736F757263655354424279746541727261794163636573736F7250726F78790000000072000000AF0C0000215354422031204E080C000A0000005354425669657750726F7879000000009A000000000000005200000010000000446F6C7068696E204D5650204261736552000000090000005368656C6C56696577620000001B000000000000000000000062000000020000000100980101000200A001000000000000000000000000000007060000000000000000000000000000A001000000000000EA000000000000000001000062000000040000009A010000000000009A0000000000000052000000080000007074323039323233520000000B0000004772614D61706156696577620000001300000000000000A00100006200000002000000820000000400000000000044010000002002000000000000000000000000000007000000000000000000000000000000200200000100000001000000000000000000000000000000000000004100000006010F004D65737361676553657175656E636500000000CA00000000000000D0000000620000000200000006030B004D65737361676553656E6400000000BA00000000000000520000001000000063726561746541743A657874656E743A620000000200000006020500506F696E7400000000C3010000150000002203000000000000890400008904000020020000D202000000000000BA000000000000005200000005000000746578743A620000000100000052000000040000004D6170612002000006010F0057494E444F57504C4143454D454E5400000000720000002C0000002C0000000000000001000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE10000000A000000250300004E020000CA00000000000000D000000062000000000000002203000000000000C1000000C1000000000000001300000052000000040000004D6170619A010000000000009A00000000000000C001000052000000110000004D756C74696C696E655465787445646974620000001000000000000000A00100006200000002000000820000000400000004110144010000001004000000000000460103000100000052474200000000FFFFFF010000000007040000000000000000000000000000100400000000000082000000080000004502FFFF0000000006020D004E756C6C436F6E766572746572000000000000000000000000090000009202000000000000CA00000000000000D00000006200000004000000D202000000000000F00200006200000002000000220300000000000015000000D30000002203000000000000910100005902000010040000D202000000000000BA00000000000000520000000A0000006973456E61626C65643A62000000010000002000000010040000D202000000000000BA00000000000000520000000F00000073656C656374696F6E52616E67653A620000000100000006030800496E74657276616C0000000003000000010000000300000010040000D202000000000000BA00000000000000520000000F0000006973546578744D6F6469666965643A62000000010000002000000010040000A203000000000000720000002C0000002C0000000000000001000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0A00000069000000D200000095010000CA00000000000000D0000000E0030000F003000000000000130000005200000003000000747874000000000000000000000000000000000000000001000000000000000000000000000000000000000100000000000000000000009202000000000000CA00000000000000D00000006200000003000000D202000000000000F0020000620000000200000022030000000000000B0000000B000000220300000000000011090000C3030000A0010000D20200000000000060030000620000000100000052000000060000004772612E2E2EA0010000D202000000000000BA0000000000000052000000080000006D656E754261723A620000000100000000000000A0010000A203000000000000720000002C0000002C0000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF05000000050000008D040000E6010000CA00000000000000D00000006200000006000000100400009A010000000000009A00000000000000C0010000520000000A00000050757368427574746F6E620000001100000000000000A00100006200000002000000820000000400000000208144010004007007000000000000000000000000000007040000000000000000000000000000700700000000000082000000080000004702FFFF000000004605120004000000436F6D6D616E644465736372697074696F6E00000000BA00000000000000520000000A00000077637A7974616A477265520000000B00000057637A7974616A2067726501000000010000000000000000000000010000009202000000000000CA00000000000000D00000006200000003000000D202000000000000F00200006200000002000000220300000000000015000000150000002203000000000000910100003300000070070000D2020000000000004005000062000000010000002000000070070000D202000000000000600300006200000001000000520000000B00000057637A7974616A2067726570070000A203000000000000720000002C0000002C0000000000000001000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0A0000000A000000D200000023000000CA00000000000000D0000000E0030000F00300000000000013000000200200009A0100000000000080070000620000001100000000000000A00100006200000002000000820000000400000000208144010004002009000000000000000000000000000007000000000000000000000000000000200900000000000082000000080000004702FFFF00000000E207000000000000BA0000000000000052000000070000006E6F776147726152000000080000004E6F77612067726101000000010000000000000000000000010000009202000000000000CA00000000000000D00000006200000003000000D202000000000000F00200006200000002000000220300000000000015000000510000002203000000000000910100003300000020090000D2020000000000004005000062000000010000002000000020090000D20200000000000060030000620000000100000052000000080000004E6F77612067726120090000A203000000000000720000002C0000002C0000000000000001000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0A00000028000000D200000041000000CA00000000000000D0000000E0030000F003000000000000130000009A0100000000000080070000620000001100000000000000A0010000620000000200000082000000040000000020814401000400A00A000000000000000000000000000007000000000000000000000000000000A00A00000000000082000000080000004702FFFF00000000E207000000000000BA000000000000005200000009000000706F6D696E54757265520000000A000000506F6D696E207475726501000000010000000000000000000000010000009202000000000000CA00000000000000D00000006200000003000000D202000000000000F002000062000000020000002203000000000000150000003F03000022030000000000009101000033000000A00A0000D20200000000000040050000620000000100000020000000A00A0000D202000000000000600300006200000001000000520000000A000000506F6D696E2074757265A00A0000A203000000000000720000002C0000002C0000000000000001000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0A0000009F010000D2000000B8010000CA00000000000000D0000000E0030000F003000000000000130000009A0100000000000080070000620000001100000000000000A0010000620000000200000082000000040000000020814401000400200C000000000000000000000000000007000000000000000000000000000000200C00000000000082000000080000004702FFFF00000000E207000000000000BA000000000000005200000005000000706F6D6F635200000005000000506F6D6F6301000000010000000000000000000000010000009202000000000000CA00000000000000D00000006200000003000000D202000000000000F002000062000000020000002203000000000000150000008D00000022030000000000009101000033000000200C0000D20200000000000040050000620000000100000020000000200C0000D2020000000000006003000062000000010000005200000005000000506F6D6F63200C0000A203000000000000720000002C0000002C0000000000000001000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0A00000046000000D20000005F000000CA00000000000000D0000000E0030000F00300000000000013000000F00300000000000015000000460504000300000049636F6E0000000000000000100000000E02110053544253696E676C65746F6E50726F7879000000009A000000000000005200000007000000446F6C7068696E5200000018000000496D61676552656C617469766546696C654C6F6361746F72BA00000000000000520000000700000063757272656E74520000000D0000005368656C6C566965772E69636F0E021F0053544245787465726E616C5265736F757263654C69627261727950726F7879000000005200000010000000646F6C7068696E64723030352E646C6C00000000'))!

