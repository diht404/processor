g++ -c sfml_test.cpp
g++ sfml_test.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
./sfml-app
rm sfml_test.o
rm sfml-app