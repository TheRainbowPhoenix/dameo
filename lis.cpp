#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>

class Board {
public:
	int ** board;
	int size;
	int turn;
	Board();
	Board(int sz);
	~Board();
	void print();
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
		for(int i = 0; i < size; i++) delete [] board[i];
		size = 0;	
}

void Board::print() {
	for(int x=0; x < size; x++) {
		for(int y=0; y<size; y++) std::cout << board[x][y] << " ";
		std::cout << std::endl;
	}		
}

int Board::unpak(char* fname) {
	char buffer [2];
	FILE *pfile;
	pfile=fopen(fname,"r");
	if (!pfile) perror ("Error opening file");
	else {
			if ( fgets (buffer , 1 , pfile)[0] != 0x7F ) return 2; // Bad Header
			else {
				turn = (int)(fgets (buffer , 1 , pfile));
				size = (int)((fgets (buffer , 1 , pfile)[0]<<8)+(fgets (buffer , 1 , pfile)[0]));
				std::cout << turn << " " << size << "\n";
			}
		while ( ! feof (pfile) ) {
			if ( fgets (buffer , 2 , pfile) == NULL ) break;
			fputs (buffer , stdout);
		}
	fclose (pfile);
	}
	return 0;
}

int Board::pak() {
	FILE *ptr_myfile;
	ptr_myfile=fopen("test01.dat","wb");
	if (!ptr_myfile) {
		printf("Unable to open file!");
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
			std::cout <<  i << " ";
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
		std::cout << (int)out[i] << " ";
		fwrite(&out[i], 1, 1, ptr_myfile);
	}
//	fwrite(&out, 4+size*(size/2)+size+3, 1, ptr_myfile);
	fclose(ptr_myfile);
	return 0;	
}

std::string Board::gen() {
	for(int j =0; j<16; j++) {
		switch(j%4) {
				case 0:
					std::cout << " ";
					break;
				case 1:
					std::cout << "▀";
					break;
				case 2:
					std::cout << "▄";
					break;
				case 3:
					std::cout << "█";
					break;
			}
			switch(j>>2) {
				case 0:
					std::cout << " ";
					break;
				case 1:
					std::cout << "▀";
					break;
				case 2:
					std::cout << "▄";
					break;
				case 3:
					std::cout << "█";
					break;
		}
		std::cout << " " << j << " ";
	}
	std::cout << std::endl;
	int i = 0;
	std::string s;
	for(int x=0; x < size; x+=2) {
		for(int y=0; y<size; y+=2) {
			i = (((board[x][y])?(1):(0))+4*((board[x][y+1])?(1):(0))+2*((board[x+1][y])?(1):(0))+8*((board[x+1][y+1])?(1):(0)))%16;
			std::cout <<  i << " ";
		}
		std::cout << "\n";
	}
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
	//std::cout << s << std::endl;
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
	ptr_myfile=fopen("test.bin","wb");
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
	Board b(8);
	Board b2;
	std::cout << b2.unpak("test01.dat") << std::endl;
	b.print();
	std::cout << b.gen();
	b.pak();
	fwri();
	FILE * pFile;
	char buffer [100];
	pFile = fopen ( "saves/test.sav" , "wb" );
	fputs ( "▀██████▀\n  ▀▀▀▀  \n  ▄▄▄▄  \n▄██████▄\n" , pFile );
	//fseek ( pFile , 5 , SEEK_SET );
	//fputs ( "0" , pFile );
	fclose ( pFile );
	list_dir("saves");
	pFile = fopen ("saves/test.sav" , "r");
	if (pFile == NULL) perror ("Error opening file");
	else {
		while ( ! feof (pFile) ) {
			if ( fgets (buffer , 100 , pFile) == NULL ) break;
			fputs (buffer , stdout);
		}
	fclose (pFile);
	}
	return 1;	
}
