
test1() {
    cp data/one/code.cpp src/main.cpp
    bash scripts/build.sh
    time valgrind bin/vector_run >test.out
    diff test.out data/one/answer.txt
}

test2() {
    cp data/two/code.cpp src/main.cpp
    bash scripts/build.sh
    time valgrind bin/vector_run >test.out
    diff test.out data/two/answer.txt
}

test3() {
    cp data/three/code.cpp src/main.cpp
    bash scripts/build.sh
    time valgrind bin/vector_run >test.out
    diff test.out data/three/answer.txt
}

test4() {
    cp data/four/code.cpp src/main.cpp
    bash scripts/build.sh
    time valgrind bin/vector_run >test.out
    diff test.out data/four/answer.txt
}

test2
