#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>

using namespace std;

class Zipf
{
private:
	int N;
	double alpha;
	double q;
	double bottom;

public:

	Zipf(int N, double alpha, double q = 0.0)
	{
		this->N = N;
		this->alpha = alpha;
		this->q = q;
		bottom = 0.0;
		for(int i = 1; i <= N; i++)
			bottom += (1 / pow( (double)i + q, alpha) );
	}

	void setSeed(unsigned int seed = 0)
	{
		if( seed == 0 )
			srand(time(NULL));

		else
			srand(seed);
	}

	int nextInt()
	{
		int rank;
		double freq = 0.0;
		double dice = (double)rand() / (double)RAND_MAX;

		rank = rand() % N;
		freq = (1.0 / pow( (double)rank + q, alpha ) ) / bottom;

		while( !(dice < freq) )
		{
			rank = rand() % N;
			freq = (1.0 / pow( (double)rank + q, alpha) ) / bottom;
			dice = (double)rand() / (double)RAND_MAX;
		}

		return rank;
	}
};

vector <string> devices;
vector <string> home_agents;

int read_devices(char* device_file)
{
	int lines = 0;
	FILE* fp = fopen(device_file, "r");
	char *buffer = new char[500];
	string device_name = "";
	devices.clear();

	while(fgets(buffer, 500, fp) != NULL)
	{
		++lines;
		char* p = strtok(buffer,",");
		device_name = p;
		device_name = "." + device_name;
		p = strtok(NULL, ",");
		device_name = p + device_name;
		//device_name += ".mypc0.";
		devices.push_back(device_name);
		if(lines >= 50) break;
	}

	fclose(fp);
	delete[] buffer;
	return lines;
}

int read_home_agents(char* nodes_file)
{
	int lines = 0;
	FILE* fp = fopen(nodes_file, "r");
	char *buffer = new char[500];
	string ha_name = "";
	home_agents.clear();

	fgets(buffer, 500, fp);

	while(fgets(buffer, 500, fp) != NULL)
	{
		++lines;
		char *p = strtok(buffer, " ");
		p = strtok(NULL," ");
		p = strtok(NULL," ");
		p = strtok(NULL," ");

		ha_name = p;
		if( ha_name[ (int)ha_name.size() - 1 ] == '\n' )
			ha_name = ha_name.substr(0, (int)ha_name.size() - 1);
		ha_name = "." + ha_name;
		ha_name += ".dht.pl.pwebproject.net";
		home_agents.push_back(ha_name);
	}

	delete[] buffer;
	fclose(fp);

	return lines;
}
int main()
{
	int total = 2000;
	devices.clear();
	int N = read_devices("device.dat");
	int n_ha = read_home_agents("nodes");

	Zipf zipf(N, 1.02, 100);
	Zipf ha_zipf(n_ha, 1.02, 100);

	zipf.setSeed();
	ha_zipf.setSeed();

	FILE* fp = fopen("query_sequence.dat", "w");
	int next_ha, next_device;

	while(total--)
	{
		next_device = zipf.nextInt();
		next_ha = ha_zipf.nextInt();

		fprintf(fp, "%s\n", (devices[next_device] + home_agents[next_ha]).c_str());
//		fprintf(fp, "%s\n", devices[next_device].c_str());
	}
	fclose(fp);
	return 0;
}
