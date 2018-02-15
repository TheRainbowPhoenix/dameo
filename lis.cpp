#include <cstdio>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>

/*
	Unusual TODO list :

	- hash to check sended data ( + re sending size before parsing ?)
	- full crypto
	- websocket ?
	- chunck cutting (4-sized item blocs)
	- chat saving, history recording ?
	- variable protection ?
	- handshake
	- data analysis ? ( tracker API)
	- INI parsing lib ? (engine settings, game stats, unimportant data that could be modified)
	- Browser backend ??? with game preview options ? Async JS with json api ?
	- !!! AI !!!
	- global ranking and other online stuff
	- friends options, group chat ...
	*/

class Board {
public:
	int ** board;
	int size;
	int turn;
	Board();
	Board(int sz);
	~Board();
	void print();
	// Modificators
	int mmove(int px, int py, int x, int y); // Return code depending case avaible or not
	// Generators
	std::string gen();
	int pak();
	int unpak(char* fname);
};

Board::Board() {
	std::cout << "Warn : unallocated" << std::endl;
	turn = 1;
}

Board::Board(int sz) {
	size = (sz%4!=0)?(sz-sz%4+4):(sz);
	turn = 1;
	board = new int*[size];
	for(int i = 0; i < size; i++) board[i] = new int[size];
	for(int x=0; x < size; x++) for(int y=0; y<size; y++) board[x][y]=((y>=x && x< size-y) && (x+1<size/2)?(1):(((y<=x && x+1>= size-y) && (x>size/2))?(2):(0)));
}

Board::~Board() {
		//for(int i = 0; i < size; i++) delete [] board[i];
		size = 0;
}

void Board::print() {
	for(int x=0; x < size; x++) {
		for(int y=0; y<size; y++) std::cout << board[x][y] << " ";
		std::cout << std::endl;
	}
}

int Board::mmove(int px, int py, int x, int y) {
	if(px>size || py>size || x>size || y>size) return 0;
	int o = board[px][py];
	int des = board[x][y];
	int dy = py-y;
	int dx = px-x;
	std::cout << o << " " << des << " - " << dx << " " << dy << std::endl;
	std::cout << "["<< px << " " << py << "] - [" << x << " " << y << "]" << std::endl;

	if(px==x && py==y) return 1;
	if((dx+3)%3 == o) return 1;
	
	if(o==1 && dx==1) return 1;
	
	if(y < 0 && dy ==1) {
		return o;
	}
	if(y > size-1 && dy ==-1) {
		return o;
	}
	if (y >= size || x >= size) return o;
	if(y < 0 || x < 0) return o;
	
	if(o == des) {
		//std::cout << "=";
		if(dy == 0) {
			//std::cout << "0";
			des = mmove(x, y, x-dx, y);
			board[x][y]=o;
			board[px][py]=des;
			return des;
			//return mmove(x, y, x-dx, y);
		}
		if(dy%2==1 && dx%2==1) {
			//std::cout << "%";
			des = mmove(x, y, x-dx, y-dy);
			//std::cout << des;
			board[x][y]=o;
			board[px][py]=des;
			return des;
		}
		if(dx == 1 && dy == -1) {
			//std::cout << "-";
			des = mmove(x, y, x-dx, y-dy);
			//std::cout << des;
			board[x][y]=o;
			board[px][py]=des;
			return des;		
		}
		
		//std::cout << "#";
		return 1;
	} else {
		//std::cout << "e";
		if(o == 0) return 1;
		if ((des != 0 && des != o) && (board[x-dx][y] == 0)) {
				//std::cout << "0";
				board[x][y]=0;
				board[px][py]=0;
				board[x-dx][y]=o;
				return 0;
			}
		if (des != 0 && des != o) {
			//std::cout << "!";
			if (board[x-dx][y-dy] == 0) {
				if (y-dy < 1) return o;
				board[x][y]=0;
				board[px][py]=0;
				board[x-dx][y-dy]=o;
				return 0;
			} else {
				//std::cout << o << " ";
				return o;
			}
		}
		if(des == 0) {
			board[x][y]=o;
			board[px][py]=des;
		}
	}
	

	return 0;
}

int Board::unpak(char* fname) {
	char *buffer, b1, b2;
	buffer = new char[3];
	int in = 0;
	int x=  -1;
	FILE *pfile;
	pfile=fopen(fname,"r");
	long pfsz = 0;
	if (!pfile) perror ("Error opening file");
	else {
		fseek(pfile, 0, SEEK_END);
		pfsz=ftell(pfile);
		rewind(pfile);
		//std::cout << pfsz << std::endl;
		fread(&b1, 1, 1, pfile);
		if(b1 != 127) {
			std::cout << "Corrupted file ...\n";
			fclose(pfile);
			return 2; // Bad Header
		} else {
			fread(&b2, 1, 1, pfile);
			turn = (int)b2;
			fread(buffer, 2, 1, pfile);
			size = (int)((buffer[1] & 0x00FF)+((buffer[0] & 0x00FF) <<8) & 0xFFFF);
      board = new int*[size];
      for(int i = 0; i < size; i++) board[i] = new int[size];
		}
		//std::cout << turn << " " << size << "\n";
		for (int i = 0; i<((((size*size)>>1)+(((size>>1)+1)<<1)))>>1; i++) {
			fread(buffer, 2, 1, pfile);
			in = (int)((buffer[1] & 0x00FF)+((buffer[0] & 0x00FF) <<8) & 0xFFFF);
			if(in==625) x++;
			else {
				//(board[x][y]+25*(board[x][y+1])+5*(board[x+1][y])+125*(board[x+1][y+1]));
				//std::cout << 2*x << " " << 2*((i%5)-1) << "\t";
				//std::cout << 2*x << "-" << 2*((i%5)-1) << " " << in%5 << "\t";
				//std::cout << 2*x << "-" << 2*((i%5)-1)+1 << " " << in/25%5 << "\t";
				board[2*x][2*((i%((size>>1)+1))-1)]=in%5;
				board[2*x][2*((i%((size>>1)+1))-1)+1]=in/25%5;
				//std::cout << 2*x+1 << "-" << 2*((i%5)-1) << " " << in/5%5 << "\t";
				//std::cout << 2*x+1 << "-" << 2*((i%5)-1)+1 << " " << in/125%5 << "\n";
				board[2*x+1][2*((i%((size>>1)+1))-1)]=in/5%5;
				board[2*x+1][2*((i%((size>>1)+1))-1)+1]=in/125%5;
				//std::cout << x << " " << (i-1)%5 << " [" << in%5 << " "<< in/5%5 << " " << in/25%5 << " " << in/125%5 << "]\n";
			}
     // std::cout << i << " ";
		}
		/*
		fread(&buffer, 1, 1 , pfile);
		if(buffer != 0x7F ){
			std::cout << (int)buffer << std::endl;
			fclose(pfile);
			return 2; // Bad Header
		} else {
			turn = (int)(fgets(&buffer , 1 , pfile));
			fread(&buffer, 1, 1 , pfile);
			size = (int)(buffer+(buffer<<8));
			std::cout << turn << " " << size << "\n";
		}
		while ( ! feof (pfile) ) {
			if (fread(&buffer, 1, 1, pfile) == NULL ) break;
			//fputs(buffer , stdout);
		}
		*/
	fclose (pfile);
	}
	return 0;
}

int Board::pak() {
	FILE *ptr_myfile;
	ptr_myfile=fopen("saves/test01.dat","wb");
	if (!ptr_myfile) {
		printf("Unable to open file!");
		fclose(ptr_myfile);
		return 1;
	}

	char * out = new char[4+size*(size/2)+size+3];
	//std::cout << 4+size*(size/2)+size+2 << std::endl;
	int i = 0;
	int p = 0;
	out[p++]=0x7F;
	out[p++]=(turn & 0x00FF);
	out[p++]=(size & 0xFF00)>> 8;
	out[p++]=(size & 0x00FF);
	out[p++]=(625 & 0xFF00)>> 8;
	out[p++] = (625 & 0x00FF);
	for(int x=0; x < size; x+=2) {
		for(int y=0; y<size; y+=2) {
			i = (board[x][y]+25*(board[x][y+1])+5*(board[x+1][y])+125*(board[x+1][y+1]));
			//std::cout <<  i << " ";
			//fwrite(((i & 0xFF00)>> 8), 1, 1, ptr_myfile);
			//fwrite(((i & 0x00FF)), 1, 1, ptr_myfile);
			out[p++]=(i & 0xFF00)>> 8;
			out[p++] = (i & 0x00FF);
		}
		//fwrite(((625 & 0xFF00)>> 8), 1, 1, ptr_myfile);
		//fwrite(((625 & 0x00FF)), 1, 1, ptr_myfile);
		out[p++]=(625 & 0xFF00)>> 8;
		out[p++] = (625 & 0x00FF);
		//std::cout << out[p-1] << out[p] << "\n";
	}
	for (int i=0; i<=p; i++) {
		//std::cout << (int)out[i] << " ";
		fwrite(&out[i], 1, 1, ptr_myfile);
	}
//	fwrite(&out, 4+size*(size/2)+size+3, 1, ptr_myfile);
	fclose(ptr_myfile);
	return 0;
}

std::string Board::gen() {
	int i = 0;
	std::string s;
	for(int x=0; x < size; x+=2) {
		for(int y=0; y<size; y+=2) {
			i = (((board[x][y])?(1):(0))+4*((board[x][y+1])?(1):(0))+2*((board[x+1][y])?(1):(0))+8*((board[x+1][y+1])?(1):(0)))%16;			//std::cout <<  i << " ";
			switch(i%4) {
				case 0:
					s.append(" ");
					break;
				case 1:
					s.append("▀");
					break;
				case 2:
					s.append("▄");
					break;
				case 3:
					s.append("█");
					break;
				}
				switch(i>>2) {
				case 0:
					s.append(" ");
					break;
				case 1:
					s.append("▀");
					break;
				case 2:
					s.append("▄");
					break;
				case 3:
					s.append("█");
					break;
			}
		}
		s.append("\n");
	}
	return s;
}

static void list_dir(const char *path) {
	struct dirent *entry;
	DIR *dir = opendir(path);
	if (dir == NULL) {
		return;
	}
	while ((entry = readdir(dir)) != NULL) {
		if(entry->d_name[0] != '.') printf("%s\n",entry->d_name);
	}
	closedir(dir);
}

struct rec 	{
	int x,y,z;
};

int fwri() {
	int counter;
	FILE *ptr_myfile;
	struct rec my_record;
	ptr_myfile=fopen("struct.bin","wb");
	if (!ptr_myfile) {
		printf("Unable to open file!");
		return 1;
	}
	for ( counter=1; counter <= 10; counter++) {
		my_record.x= counter;
		fwrite(&my_record, 2, 1, ptr_myfile);
	}
	fclose(ptr_myfile);
	return 0;
}


int main() {
	int sz = 12;
	int i = 0;
	/*
	int lol = -1;
	for(lol = -1; lol < 3; lol++) {
		std::cout<<((lol+3)%3);
	}*/
	//std::cout << "sz: ";
	//std::cin >> sz;
	Board b(sz);
	b.print();
	b.pak();
	Board b2;
	b2.unpak("saves/test01.dat");
	b2.print();
	std::cout << b.gen();
	b2.mmove(4, 3, 2,3);
	b2.mmove(5, 6, 4,6);
	b2.print();
	int px, py, ix,iy;
	do {
		scanf("%d %d %d %d", &px, &py, &ix, &iy);
		if (b2.mmove(px, py, ix,iy) == 0)
			std::cout << b2.gen();
		else
			b2.print();
	} while (1);
	std::cout << b2.gen();
	b.pak();
	fwri();
	/*
	FILE * pFile;
	char buffer [100];
	pFile = fopen ( "saves/test.sav" , "wb" );
	fputs ( "▀██████▀\n  ▀▀▀▀  \n  ▄▄▄▄  \n▄██████▄\n" , pFile );
	//fseek ( pFile , 5 , SEEK_SET );
	//fputs ( "0" , pFile );
	fclose ( pFile );*/

	list_dir("saves");

	/*
	pFile = fopen ("saves/test.sav" , "r");
	if (pFile == NULL) perror ("Error opening file");
	else {
		while ( ! feof (pFile) ) {
			if ( fgets (buffer , 100 , pFile) == NULL ) break;
			fputs (buffer , stdout);
		}
	fclose (pFile);
	}*/
	return 1;
}
