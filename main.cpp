#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <math.h>

using namespace std;

#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>

vector<double> buffer;
vector<double> buffer2;

struct ss{
	unsigned int start, stop;
	double srednia;
};

vector<ss> vss;

vector<double> load(string path);

double srednia(vector<double>* data) {
	long double tmp = 0;
	for (unsigned int i = 0; i < (*data).size(); i++){
		if((*data)[i] != 0)
			tmp += (*data)[i];
	}
	if (tmp == 0 || (*data).size() == 0 || isinf(tmp))
		cout << "err" << endl;
	return tmp / (double)(*data).size();
}

int main() {

	//vss.push_back({ 100,300 });

/*for (int x = 0; x <count; x++) {
	for (int i = 0; i<256; i++) {
		int c = (buff16[i + 1] << 8) | buff16[i];
		double t = c / 32768.0;
		buffer.push_back(t);
		data[x] = t;
	}
}*/

	unsigned int data_size;
	vector<double> buffer = load("3.wav");
	vector<double> buffer2 = load("3.wav");
	vector<double> buffer3 = load("w2.wav");

	cout << "Buffer 0 size: " << buffer.size() << endl;
	cout << "Buffer 1 size: " << buffer2.size() << endl;
	cout << "Buffer 2 size: " << buffer3.size() << endl;
	cout << buffer2.size() / buffer3.size() << endl;
	cout << buffer2.size() / (buffer3.size()/4) << endl;

	int u = buffer3.size() / 5;
	for (unsigned int i = 0; i < buffer2.size(); i+=u){
		vector<double> tmp;
		for (unsigned int j = 0; j < buffer3.size(); j++){
			if (buffer2.size() > i + j) {
				double data = buffer2[i + j] / buffer3[j];
				if (!isinf(data))
					tmp.push_back(data);
			}
		}
		vss.push_back({i,i+ buffer3.size(),srednia(&tmp) });
	}

	int width = 1024;
	int height = 500;
	

	al_init();
	al_init_primitives_addon();
	al_install_keyboard();

	ALLEGRO_KEYBOARD_STATE klawiatura;
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	ALLEGRO_DISPLAY *okno = al_create_display(width, height);
	al_set_window_title(okno, "Allegro5 - Prymitywy");
	ALLEGRO_EVENT_QUEUE *kolejka = al_create_event_queue(); // event_queue
	al_register_event_source(kolejka, al_get_display_event_source(okno));
	ALLEGRO_EVENT zdarzenie; // event

	bool exit = false;
	while (!exit) {
		al_get_next_event(kolejka, &zdarzenie);
		if (zdarzenie.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { return 0; }
		al_get_keyboard_state(&klawiatura);
		al_clear_to_color(al_map_rgb(150, 150, 150)); //t³o

		al_draw_line(0, (height / 4), width, (height / 4), al_map_rgb(0, 255, 0), 1);
		if (buffer.size() > 0) {
			for (int i = 0; i < width; i++) {
				//double s = buffer[(i * buffer.size()) / width];
				double s = buffer[(i * buffer.size()) / width];
				int y = (height / 4) * s;
				al_draw_line(i, (height / 4) - y, i, (height / 4) + y, al_map_rgb(0, 255, 0), 1);
			}
		}

		al_draw_line(0, (height - (height / 4)), width, (height - (height / 4)), al_map_rgb(0, 255, 0), 1);
		if (buffer.size() > 0) {
			for (int i = 0; i < width; i++) {
				double s = buffer[(i * buffer.size()) / width];
				int y = (height / 4) * s;
				al_draw_line(i, (height - (height / 4)) - y, i, (height - (height / 4)) + y, al_map_rgb(0, 255, 0), 1);
			}

			for (unsigned int i = 0; i < vss.size(); i++){
				al_draw_line(((double)vss[i].start / (double)buffer.size())*width, (height / 2), ((double)vss[i].start / (double)buffer.size())*width, height, al_map_rgb(0, 0, 255), 2);

				if(vss[i].srednia < 0)
					al_draw_line(((double)vss[i].start / (double)buffer.size())*width, (height - (height / 1.9)), ((double)vss[i].stop / (double)buffer.size())*width, (height - (height / 1.9)), al_map_rgb(255, 255, 0), (vss[i].srednia * -1)*3);
				else
					al_draw_line(((double)vss[i].start / (double)buffer.size())*width, (height - (height / 1.9)), ((double)vss[i].stop / (double)buffer.size())*width, (height - (height / 1.9)), al_map_rgb(255, 0, 0), vss[i].srednia*3);
			}
		}


		al_flip_display();
		al_rest(0.005);
	}

	return 0;
}

vector<double> load(string path){
	FILE* f = fopen(path.c_str(), "rb");

	vector<double> tmp;

	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;

	fread(type, sizeof(char), 4, f);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') {
		fclose(f);
		return tmp;
	}

	fread(&size, sizeof(DWORD), 1, f);
	fread(type, sizeof(char), 4, f);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') {
		fclose(f);
		return tmp;
	}

	fread(type, sizeof(char), 4, f);
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') {
		fclose(f);
		return tmp;
	}

	fread(&chunkSize, sizeof(DWORD), 1, f);
	fread(&formatType, sizeof(short), 1, f);
	fread(&channels, sizeof(short), 1, f);
	fread(&sampleRate, sizeof(DWORD), 1, f);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, f);
	fread(&bytesPerSample, sizeof(short), 1, f);
	fread(&bitsPerSample, sizeof(short), 1, f);

	fread(type, sizeof(char), 4, f);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a') {
		fclose(f);
		return tmp;
	}

	fread(&dataSize, sizeof(DWORD), 1, f);

	unsigned char* buf = new unsigned char[dataSize];
	fread(buf, sizeof(BYTE), dataSize, f);

	if (channels == 2) {
		//(*out_data) = new double[dataSize / 4];
		//unsigned j = 0;
		for (unsigned i = 0; i < dataSize; i += 4) {
			short left = *(short *)&buf[i];
			short right = *(short *)&buf[i + 2];
			short monoSample = (int(left) + right) / 2;
			double t = monoSample / 32768.0;
			//(*out_data)[j] = t;
			//j++;
			tmp.push_back(t);
		}
	}
	else if (channels == 1) {
		//(*out_data) = new double[dataSize];
		for (DWORD i = 0; i < dataSize; i++) {
			short c = *(short *)&buf[i];
			double t = c / 32768.0;
			//(*out_data)[i] = t;
			tmp.push_back(t);
		}
	}
	else
		return tmp;

	return tmp;
}