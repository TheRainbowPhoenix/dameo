#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>

class Board {
public:
	int ** board;
	int size;
	bool turn;
	Board(int sz);
	~Board();
	void print();
};

Board::Board(int sz) {
	size = (sz%2)?(sz+1):(sz);
	board = new int*[size];
	for(int i = 0; i < size; i++) board[i] = new int[size];
	for(int x=0; x < size; x++) {
		for(int y=0; y<size; y++) {
			//(y>=x && x< sz-y) && (x+1<size/2)
			board[x][y]=((y>=x && x< sz-y) && (x+1<size/2)?(1):(((y<=x && x+1>= sz-y) && (x>size/2))?(2):(0)));
			//if(((y>=x && x< sz-y) && (x+1<size/2)) || ((y<=x && x+1>= sz-y) && (x>size/2))) board[x][y]=1;
			//else board[x][y]=0;
		}
	}	
}

Board::~Board() {
		for(int i = 0; i < size; i++) delete [] board[i];
		size = 0;	
}

void Board::print() {
	for(int x=0; x < size; x++) {
		for(int y=0; y<size; y++) {
			std::cout << board[x][y] << " ";
		}
		std::cout << std::endl;
	}	
	
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
	Board b(12);
	b.print();
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