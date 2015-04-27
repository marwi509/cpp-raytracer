all:
	g++ -o main main.cpp Image.cpp Vector3.cpp Object.cpp Scene.cpp Polygon.cpp Cell.cpp Light.cpp Noise3.cpp Point.cpp -O3 -s
fast:
	g++ -o main main.cpp LargeHdrImage.cpp Image.cpp Vector3.cpp Object.cpp Scene.cpp Polygon.cpp Cell.cpp Light.cpp Noise3.cpp Point.cpp -Ofast -s -flto -fwhole-program -fno-signed-zeros -march=native -fopenmp

opt:
	/opt/intel/bin/icc -o main main.cpp Image.cpp Vector3.cpp Object.cpp Scene.cpp Polygon.cpp Cell.cpp Light.cpp
