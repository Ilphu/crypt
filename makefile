.PHONY: all

all: crypt.gb

crypt.gb: game.o main.o graphics.o character.o enemies.o
	rgblink --dmg --tiny --map crypt.map --sym crypt.sym -o crypt.gb main.o game.o graphics.o character.o enemies.o
	rgbfix -v -p 0xFF crypt.gb
	rm game.o main.o graphics.o character.o enemies.o

enemies.o: enemies.asm include/*.inc graphics/*.tlm graphics/*.chr
	rgbasm -o enemies.o enemies.asm

character.o: character.asm include/*.inc graphics/*.tlm graphics/*.chr
	rgbasm -o character.o character.asm

graphics.o: graphics.asm include/*.inc graphics/*.tlm graphics/*.chr
	rgbasm -o graphics.o graphics.asm

game.o: game.asm  include/*.inc graphics/*.tlm graphics/*.chr
	rgbasm -o game.o game.asm

main.o: main.asm include/*inc graphics/*.tlm graphics/*.chr
	rgbasm -o main.o main.asm