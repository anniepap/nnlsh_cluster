#include <iostream>
#include <sstream>
#include <string.h>
#include "evaluation.h"
#include "distanceFunctions.h"

using namespace std;

string Silhouette(ClusterInfo** clusters, string d, int number_of_clusters, int flag) {

	double dist, newDist, a, b, si, s;
	int count, secondBest, total;
	ostringstream os;
	s = 0;

	for (int i = 0; i < number_of_clusters; i++) {
		si = 0;
		total = 0;

		//for each object
		List* curves = clusters[i]->getCurves();
		Node* t = curves->getHead();
		while (t != NULL) {
			//average distance of object to other objects
			a = 0;
			count = 0;
			Node* tp = curves->getHead();
			while (tp != NULL) {
				if (d.compare("DFT") == 0)
					a += FrechetDistance(t->info, tp->info);
				else if (d.compare("DTW") == 0)
					a += WarpingDistance(t->info, tp->info);
				else
					a += cRMSD(t->info, tp->info, flag);

				count++;
				tp = tp->next;
			}
			if (count > 0)
				a = a/count;

			//find the 2nd closest centroid
			dist = 10000;
			for (int j = 0; j < number_of_clusters; j++) {
				if (i != j) {
					if (d.compare("DFT") == 0)
						newDist = FrechetDistance(t->info, clusters[j]->getCentroid());
					else if (d.compare("DTW") == 0)
						newDist = WarpingDistance(t->info, clusters[j]->getCentroid());
					else
						newDist = cRMSD(t->info, clusters[j]->getCentroid(), flag);

					if (newDist < dist) {
						dist = newDist;
						secondBest = j;
					}
				}
			}
			//average distance of object to objects in cluster of 2nd closest centroid
			b = 0;
			count = 0;
			tp = clusters[secondBest]->getCurves()->getHead();
			while (tp != NULL) {
				if (d.compare("DFT") == 0)
					b += FrechetDistance(t->info, tp->info);
				else if (d.compare("DTW") == 0)
					b += WarpingDistance(t->info, tp->info);
				else
					b += cRMSD(t->info, tp->info, flag);

				count++;
				tp = tp->next;
			}
			if (count > 0)
				b = b/count;

			//calculate object's silhouette
			if (a != 0 || b != 0)
				si += (b - a)/Max(a, b);
			total++;
			t = t->next;
		}
		//cluster's average silhouette
		if (total == 0)
			si = 0;
		else
			si = si/total;

		//os << "s" << (i+1) << ": " << si << ", ";
		s += si;
	}
	//calculate total silhouette
	s = s/number_of_clusters;
	//os << "s_total: " << s;
	os << s;

	return os.str();
}
