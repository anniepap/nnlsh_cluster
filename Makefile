SOURCE	:= proteins.cpp roads.cpp roadsLSH.cpp segmentation.cpp curve.cpp curveList.cpp limits.cpp grid.cpp gridCurve.cpp initialization.cpp clusterInfo.cpp distanceFunctions.cpp bucketList.cpp hashFunctions.cpp hashTable.cpp assignment.cpp update.cpp evaluation.cpp
OBJS	:= curve.o curveList.o limits.o grid.o gridCurve.o initialization.o clusterInfo.o distanceFunctions.o bucketList.o hashFunctions.o hashTable.o assignment.o update.o evaluation.o
HEADER	:= curve.h curveList.h limits.h grid.h gridCurve.h initialization.h clusterInfo.h distanceFunctions.h bucketList.h hashFunctions.h hashTable.h assignment.h update.h evaluation.h

all: proteins roads roadsLSH segmentation

proteins.o: proteins.cpp $(HEADER)
	g++ -c proteins.cpp
roads.o: roads.cpp $(HEADER)
	g++ -c roads.cpp
roadsLSH.o: roadsLSH.cpp $(HEADER)
	g++ -c roadsLSH.cpp
segmentation.o: segmentation.cpp distanceFunctions.h curve.h
	g++ -c segmentation.cpp -lm
curveList.o: curveList.cpp curveList.h curve.h
	g++ -c curveList.cpp
curve.o: curve.cpp curve.h limits.h
	g++ -c curve.cpp
limits.o: limits.cpp limits.h
	g++ -c limits.cpp
grid.o: grid.cpp grid.h
	g++ -c grid.cpp -lm
gridCurve.o: gridCurve.cpp gridCurve.h
	g++ -c gridCurve.cpp
initialization.o: initialization.cpp initialization.h curveList.h curve.h
	g++ -c initialization.cpp
clusterInfo.o: clusterInfo.cpp clusterInfo.h curveList.h curve.h
	g++ -c clusterInfo.cpp
distanceFunctions.o: distanceFunctions.cpp distanceFunctions.h
	g++ -c distanceFunctions.cpp -lm -I /home/despoina/Documents/eigen-eigen-5a0156e40feb
bucketList.o: bucketList.cpp bucketList.h
	g++ -c bucketList.cpp
hashFunctions.o: hashFunctions.cpp hashFunctions.h
	g++ -c hashFunctions.cpp -lm
hashTable.o: hashTable.cpp hashTable.h
	g++ -c hashTable.cpp
assignment.o: assignment.cpp assignment.h
	g++ -c assignment.cpp
update.o: update.cpp update.h
	g++ -c update.cpp
evaluation.o: evaluation.cpp evaluation.h
	g++ -c evaluation.cpp
	
proteins: proteins.o $(OBJS)
	g++ -o proteins proteins.o $(OBJS)
roads: roads.o $(OBJS)
	g++ -o roads roads.o $(OBJS)
roadsLSH: roadsLSH.o $(OBJS)
	g++ -o roadsLSH roadsLSH.o $(OBJS)
segmentation: segmentation.o $(OBJS)
	g++ -o segmentation segmentation.o $(OBJS)

clean:
	rm -f proteins roads roadsLSH segmentation proteins.o roads.o roadsLSH.o segmentation.o $(OBJS)