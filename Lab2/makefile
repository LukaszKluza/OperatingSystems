CC = gcc
CFLAGS = -Wall -O0

# Definiujemy cele
all: static shared main_static main_wspoldz main_dyn

# Cele, które nie odpowiadają rzeczywistym plikom
.PHONY: all clean

static:
# kompilacja bez linkowania i tworzenia pliku wykonywalnego
	$(CC) $(CFLAGS) -c collatz.c
# stworzenie archiwum, utworznenie, zaaktualiwoanie i dodanie collatz.o do archiwum libcollatz.a
	ar rcs libcollatz.a collatz.o

shared:
# kompilacja bez linkowania i tworzenia pliku wykonywalnego
# ("Position Independent Code", czyli kod niezależny od pozycji.)
# umożliwa elastyczne elastyczne ładowanie bibliotek do pamięci.
	$(CC) $(CFLAGS) -fPIC -c collatz.c
# Kompiluje plik obiektowy collatz.o jako bibliotekę współdzieloną libcollatz.so
	$(CC) $(CFLAGS) -shared collatz.o -o libcollatz.so

# Kompiluje plik client.c wraz z biblioteką statyczną libcollatz.a i tworzy plik wykonywalny main_static
main_static: client.c libcollatz.a
	$(CC) $(CFLAGS) client.c libcollatz.a -o main_static


# Kompiluje plik client.c, linkując z biblioteką współdzieloną libcollatz.so, ustawiając rpath na bieżący katalog, i tworzy plik wykonywalny main_wspoldz
# - L. informacja gdzie szukać biblioteki do linkowania, -l<<name>> linkowanie biblioteki o podajnej nazwie
# -Wl pozwala przekazywać argumenty bezpośrednio do linkera, -rpath=. ustawia ścieżkę w której linker będzie szukać biblioteki
main_wspoldz: client_dyn.c
	$(CC) $(CFLAGS) client.c -L. -lcollatz -Wl,-rpath=. -o main_wspoldz

# Kompiluje plik client_dyn.c, linkuje go z biblioteką libdl umożliwiającą dynamiczne ładowanie bibliotek podczas działania programu,
# i definiuje symbol preprocesora DYNAMIC
main_dyn:
	$(CC) $(CFLAGS) client_dyn.c -o main_dyn -ldl -D DYNAMIC

# Czyści wszystkie pliki tymczasowe oraz pliki wynikowe kompilacji i linkowania
clean:
	rm -f *.o *.so* *.a main_static main_wspoldz main_dyn
