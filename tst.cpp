#include <sys/ioctl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <termios.h>
#include<sys/socket.h>
#include<arpa/inet.h>

struct screen {
	int x;
	int y;
};

class term {
	public:
		int _height;
		int _width;
		screen scr;
		void SetCursorPos(int x, int y);
		bool resize();
		int *get();
		term();
		~term();
};

struct button {
	int id;
	std::string caption;
	bool cliquable;
};

struct GamePreview {
	std::string *grid;
	std::string title; 
};

class message {
	private:
		term _t;
		void _draw(int &i, int &j);
	public:
		std::string title;
		button * buttons;
		std::string label;
		bool show();
		int ask();
		message();
		message(term &t);
		message(term &t, std::string label, int type);
};

/* types :
 * I / Booleans
 * 	0 : Message
 * 	1: Error
 * 	2 : Warning
 * 	3 : Info
 * II / Binaries
 *		4 : Yes / No
 * III / Choice
 *		5 : return clicked id (int)
*/
	
class OpenDialog : public message {
	private:
		term _t;
		void _init();
		void _draw(int &i, int &j);
		void _list(int &i, int &j);
	public:
		std::string title;
		GamePreview * games;
		button * buttons;
		std::string label;
		std::string path;
		std::string Open();
		OpenDialog();
		OpenDialog(term &t);
		OpenDialog(term &t, std::string pth);
};

class board {
	int ** brd;
	bool turn;
};

class game {
	private:
		void _headDraw();
		void _footDraw();
		void _cleanHint();
	public:
		int line;
		term t;
		bool debug;
		bool inp;
		std::string title;
		screen current;
		void redraw();
		void loop();
		void setHint(std::string s);
		game();
		game(bool deb);
};

// LIB_FT 2 CPP

int ilen(unsigned long n) {
    if (n < 10) return 1;
    return 1 + ilen(n / 10);
}

// IO ROUTINES

std::string getstr(void) {
	char *p;
	char buf[254];
	int i = 0;
	std::string s;
	if (fgets(buf, sizeof(buf), stdin) != NULL) {
		s.append(buf);
	}
	return s;
}

char getch(void) {
	char buf = 0;
    struct termios old = { 0 };
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcgetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    //printf("%c\n", buf);
    return buf;
}

std::string getkey(void) {
	char *p;
	char buf[2];
	int i = 0;
	std::string s;

	while (read(0, buf, 1) > 0) {
			if (buf[0] == '\n' || buf[0] == '\r')
				break;
			else std::cout << buf << " ";
		}
	return " lol ";
}

/*	if (fgets(buf, sizeof(buf), stdin) != NULL) {
			if ((p = strchr(buf, '\n')) != NULL) {
				redraw();
				return;
			}
		}*/


// CLASSES

void OpenDialog::_init() {
	title = "Open File";
	GamePreview _new;
	_new.grid = new std::string[4];
	_new.grid[0] = "▀██████▀";
	_new.grid[1] = "  ▀▀▀▀  ";
	_new.grid[2] = "  ▄▄▄▄  ";
	_new.grid[3] = "▄██████▄";
	_new.title = "New";
	games = new GamePreview[1];
	games[0] = _new;
	button opn;
	opn.id = 1;
	opn.caption = "Open";
	opn.cliquable = true;
	button cnl;
	cnl.id = 2;
	cnl.caption = "Cancel";
	cnl.cliquable = true;
	buttons = new button[2];
	buttons[0] = cnl;
	buttons[1] = opn;
	label = "Saved";	
	path = ".";
}

OpenDialog::OpenDialog() {
	term _t;
	_init();
}

OpenDialog::OpenDialog(term &t) {
	_t =	t;
	_init();
}

OpenDialog::OpenDialog(term &t, std::string pth) {
	_t =	t;
	_init();
	path = pth; 
}

void OpenDialog::_draw(int &i, int &j) {
	i = _t.scr.x/2 - (1+2+4+3)/2;
	j = (_t.scr.y >= 48)?(_t.scr.y/8):(2);
	int k = (_t.scr.y >= 48)?(6*_t.scr.y/8):(_t.scr.y-4);
	
	_t.SetCursorPos(j,i);
	for (int l = 0; l < k/2 - title.length()/2-1; l++) std::cout << "░";
	std::cout <<" "<<title<<" ";
	for (int l = 0; l < (k/2 - title.length()/2-(title.length()%2+1)); l++) std::cout << "░";
	_t.SetCursorPos(j,++i);
	printf("\033[7m");
	k = (_t.scr.y >= 48)?(6*_t.scr.y/8):(_t.scr.y-4);
	for (int l = 0; l < k; l++) std::cout << " ";
	_t.SetCursorPos(j,++i);	
	std::cout <<  "   " << label;
	for (int y = label.length()+3; y < k; y++) std::cout << " ";	
	_t.SetCursorPos(j,++i);
	for (int l = 0; l < k; l++) std::cout << " ";
}


void OpenDialog::_list(int &i, int &j) {
	int k = (_t.scr.y >= 48)?(6*_t.scr.y/8):(_t.scr.y-4);
	//std::cout << k/12;

	for(int p = 1; p<5; p++) {
		_t.SetCursorPos(j,i++);
		std::cout << "  " << games[0].grid[p-1];
		for(int c=0; c<k/12-1; c++) std::cout << "    " << games[0].grid[p-1];
		for (int l = 12*(k/12)-2; l < k; l++) std::cout << " ";
	}
	_t.SetCursorPos(j,i++);
	std::cout << "   " << games[0].title;
	for(int c=0; c<k/12-1; c++) {
		for(int c=0; c<11-games[0].title.length(); c++) std::cout << " ";
		std::cout << " " << games[0].title;
	}
	for(int c=0; c<11-games[0].title.length(); c++) std::cout << " ";
	for (int l = 12*(k/12)+2; l < k; l++) std::cout << " ";

}

std::string OpenDialog::Open() {
	_t.resize();
	int i;
	int j;
	int k;
	int w = (_t.scr.y >= 48)?(6*_t.scr.y/8):(_t.scr.y-4);

	char ch;
	_draw(i, j);	
	_list(i, j);	

	//░▒ ▓ █ ▀ ▄ ■
	
	_t.SetCursorPos(j,i);
	for (size_t l = 0; l <w; l++)	std::cout << " ";
	_t.SetCursorPos(j,++i);	
	k=(w/2 - buttons[0].caption.length())/2-2;
	for (size_t l = 0; l <k; l++)	std::cout << " ";
	printf("\033[27m");
	std::cout <<  "[ " << buttons[0].caption << " ]";
	printf("\033[7m");
	for (size_t l = k+buttons[0].caption.length()+4; l <w/2; l++)	std::cout << " ";
	k=(w/2 - buttons[1].caption.length())/2-2;
	for (size_t l = 0; l <k; l++)	std::cout << " ";
	printf("\033[27m");
	std::cout <<  "[ " << buttons[1].caption << " ]";
	printf("\033[7m");
	for (size_t l = k+buttons[1].caption.length()+4; l <w/2; l++)	std::cout << " ";
	if((buttons[0].caption.length()+buttons[1].caption.length())%2)  std::cout << " ";
	_t.SetCursorPos(j,++i);	
	for (int l = 0; l < w; l++) std::cout << " ";
	printf("\033[27m");	
	_t.SetCursorPos(j,i+1);	
	for (int l = 0; l < w; l++) std::cout << " ";

	_t.SetCursorPos((_t.scr.y-w)/2+1, i-1);

	char ke = 0;
	int rtrn = 1;
	k = (_t.scr.y >= 48)?(_t.scr.y/8+k+1):(3+k);
	j =	buttons[0].caption.length()+((_t.scr.y)%2) + (w/2 - (buttons[1].caption.length()+1)) - ((_t.scr.y >= 48)?(1):(0));
	
	_t.SetCursorPos(k, i-1);
		
	while ((ch = getch()) != 10)
	{
		if(ch == 121) return "1";
		if(ch == 110) return "2";
		if(ch == 27) ke = 1;
		if(ch == 91 && ke == 1) ke = 2;
		if(ch == 67 && ke == 2) {
			if(rtrn != 2) k+=j;
			rtrn=2;
		}
		if(ch == 68 && ke == 2) {
			if(rtrn == 2) k-=j;
			rtrn=1;
		}
		if(ch != 27 && ch != 91) ke = 0;
		if(ch == 10) break;
		_t.SetCursorPos(k, i-1);
	}
	std::cout << rtrn;

	return "New";
}


message::message() {
	term _t;
	title = "Message";
	button ok;
	ok.id = 1;
	ok.caption = "Ok";
	ok.cliquable = true;
	buttons = new button[1];
	buttons[0] = ok;
	label = "Please confirm your action";
}

message::message(term &t) {
	_t =	t;
	title = "Message";
	button ok;
	ok.id = 1;
	ok.caption = "Ok";
	ok.cliquable = true;
	buttons = new button[1];
	buttons[0] = ok;
	label = "Oops something is missing ...";

}

message::message(term &t, std::string lbl, int type) {
	_t = t;
	button ok;
	ok.id = 1;
	ok.caption = "Ok";
	ok.cliquable = true;
	button yes;
	yes.id = 2;
	yes.caption = "Yes";
	yes.cliquable = true;
	button no;
	no.id = 3;
	no.caption = "No";
	no.cliquable = true;
	
	switch(type) {
		default:
		case 0:
			title = "Message";
			buttons = new button[1];
			buttons[0] = ok;
			label = (lbl.empty())?("I'm a message o/"):lbl;					
			break;
		case 1:
			title = "Warning";
			buttons = new button[1];
			buttons[0] = ok;
			label = (lbl.empty())?("I'm a message o/"):lbl;					
			break;
		case 2:
			title = "Error";
			buttons = new button[1];
			buttons[0] = ok;
			label = (lbl.empty())?("I'm a message o/"):lbl;					
			break;
		case 3:
			title = "Info";
			buttons = new button[1];
			buttons[0] = ok;
			label = (lbl.empty())?("I'm a message o/"):lbl;					
			break;
		case 4:
			title = "Question";
			buttons = new button[2];
			buttons[0] = yes;
			buttons[1] = no;
			label = (lbl.empty())?("This sentence is false ..."):lbl;					
			break;
		}				

}

void message::_draw(int &i, int &j) {
	i = _t.scr.x/2 - (1+3+2)/2;
	j = (_t.scr.y >= 48)?(_t.scr.y/4):(2);
	int k = (_t.scr.y >= 48)?(_t.scr.y/4):(_t.scr.y/2-2);
	
	_t.SetCursorPos(j,i);
	for (int l = 0; l < k - title.length()/2-1; l++) std::cout << "░";
	std::cout <<" "<<title<<" ";
	for (int l = 0; l < k - title.length()/2-(title.length()%2+((j==2)?(1):(0))); l++) std::cout << "░";
	_t.SetCursorPos(j,++i);
	printf("\033[7m");
	k = (_t.scr.y >= 48)?(_t.scr.y/2):(_t.scr.y-4);
	for (int l = 0; l < k; l++) std::cout << " ";
	_t.SetCursorPos(j,++i);	
	std::cout <<  " " << label;
	for (int y = label.length()+1; y < k; y++) std::cout << " ";	
	_t.SetCursorPos(j,++i);
	for (int l = 0; l < k; l++) std::cout << " ";
		
}

int message::ask() {
	_t.resize();
	int i;
	int j;
	int k;
	int w = (_t.scr.y >= 48)?(_t.scr.y/2):(_t.scr.y-4);
	
/*		if (s.length() >= t.scr.y-4) {
		s = s.substr(0, (_t.scr.y-5	));
	}*/

	char ch;
	_draw(i, j);	

	_t.SetCursorPos(j,++i);	
	k=(w/2 - buttons[0].caption.length())/2-2;
	for (size_t l = 0; l <k; l++)	std::cout << " ";
	printf("\033[27m");
	std::cout <<  "[ " << buttons[0].caption << " ]";
	printf("\033[7m");
	for (size_t l = k+buttons[0].caption.length()+4; l <w/2; l++)	std::cout << " ";
	k=(w/2 - buttons[1].caption.length())/2-2;
	for (size_t l = 0; l <k; l++)	std::cout << " ";
	printf("\033[27m");
	std::cout <<  "[ " << buttons[1].caption << " ]";
	printf("\033[7m");
	for (size_t l = k+buttons[1].caption.length()+4; l <w/2; l++)	std::cout << " ";
	if((buttons[0].caption.length()+buttons[1].caption.length()+((_t.scr.y >= 48)?(0):(1)))%2)  std::cout << " ";
	_t.SetCursorPos(j,++i);	
	for (int l = 0; l < w; l++) std::cout << " ";
	printf("\033[27m");	
	_t.SetCursorPos(j,i+1);	
	for (int l = 0; l < w; l++) std::cout << " ";

	_t.SetCursorPos(w/2+k+1, i-1);

	char ke = 0;
	int rtrn = 1;
	k = (_t.scr.y >= 48)?(_t.scr.y/4+k+1):(4+k);
	j =	buttons[0].caption.length()+((_t.scr.y)%2) + (w/2 - (buttons[1].caption.length()+1));
	
	_t.SetCursorPos(k, i-1);
		
	while ((ch = getch()) != 10)
	{
		if(ch == 121) return 1;
		if(ch == 110) return 2;
		if(ch == 27) ke = 1;
		if(ch == 91 && ke == 1) ke = 2;
		if(ch == 67 && ke == 2) {
			if(rtrn != 2) k+=j;
			rtrn=2;
		}
		if(ch == 68 && ke == 2) {
			if(rtrn == 2) k-=j;
			rtrn=1;
		}
		if(ch != 27 && ch != 91) ke = 0;
		if(ch == 10) break;
		_t.SetCursorPos(k, i-1);
	}
	std::cout << rtrn;
	return rtrn;
}

bool message::show() {	
	_t.resize();
	int i;
	int j;
	int k;
	int w = _t.scr.y/2;
	char ch;
	_draw(i, j);	
	if(_t.scr.y < 48) w=_t.scr.y-4;
	_t.SetCursorPos(j,++i);	
	k=(w - buttons[0].caption.length())/2-2;
	for (size_t l = 0; l <k; l++)	std::cout << " ";
	printf("\033[27m");
	std::cout <<  "[ " << buttons[0].caption << " ]";
	printf("\033[7m");
	for (size_t l = k+buttons[0].caption.length()+4; l <w; l++)	std::cout << " ";
	_t.SetCursorPos(j,++i);	
	for (int l = 0; l < w; l++) std::cout << " ";
	printf("\033[27m");
	_t.SetCursorPos(j,i+1);	
	for (int l = 0; l < w; l++) std::cout << " ";

	
	_t.SetCursorPos(k = _t.scr.y/2-(buttons[0].caption.length()/2+(_t.scr.y%2)), i-1);
	while ((ch = getch()) != 10)
	{
		_t.SetCursorPos(k, i-1);
		if(ch == 10) break;
	}
	return true;
}

//message::message(std::string title, button * buttons, std::string label) {}

game::game() {
	term t;
	debug =  false;
	title="Dameo Game";
	_headDraw();
	_cleanHint();
	_footDraw();
	inp = true;
}

game::game(bool deb) {
	debug = deb;
	term t;
	title="Dameo Game";
	if(debug) {
		title.append(" [Debug]");
	}
	//_headDraw();
	//_footDraw();
	//t.SetCursorPos(0,t.scr.y);
	loop();
}

void game::_headDraw() {

	int i;
	int j;

	t.SetCursorPos(0,0);
	std::cout << " ▀▄ ";
	for (int y = 4; y < t.scr.y; y++) std::cout << "░";
	if(debug) {
		i = t.scr.y - (ilen(t.scr.x)+ilen(t.scr.y)+2);
		t.SetCursorPos(i,0);
		std::cout << " " << t.scr.x << "-" << t.scr.y << std::endl;
	}
	j = (t.scr.y)/2 - (4+title.length()/2);
	t.SetCursorPos(j,0);
	std::cout <<" == "<<title<<" == ";
	t.SetCursorPos(0,1);

	for (size_t r = 0; r < (t.scr.y); r++) {
		std::cout << " ";
	}
	t.SetCursorPos(0,2);

}

void game::_footDraw() {
	int delim = t.scr.y;
	std::string rmsg;
	std::string lmsg;

	lmsg = "a.out";
	rmsg = "Top";

	delim -= rmsg.length();
	t.SetCursorPos(0,t.scr.x-1);
	printf("\033[7m");
	std::cout << lmsg;
	for (int y = lmsg.length(); y < delim; y++) std::cout << " ";
	std::cout << rmsg;
	printf("\033[27m");

	//t.SetCursorPos(0,0);
}

void game::_cleanHint() {
	t.SetCursorPos(0,1);
	for (size_t r = 0; r < (t.scr.y); r++) {
		std::cout << " ";
	}
}

void game::loop() {
	char *p;
	char ch = 0;
	char k = 0;
	int i = 0;
	std::string s = "";
	char buf[254];
	message m(t, "Lol i'm a message :D", 1);
	while (m.show() != true);
	message q(t, "Do U know da wae ?", 4);
	while ((i = q.ask()) != 1);

	OpenDialog opn(t, "Saves");
	while((s = opn.Open()) == "");
	
	redraw();
	_headDraw();
	_footDraw();
	
	while (m.show() != true);

	redraw();
	_headDraw();
	_footDraw();
	
	while ((i = q.ask()) != 1);

	redraw();
	_headDraw();
	_footDraw();
	
	while (m.show() != true);
		
	redraw();
	_headDraw();
	_footDraw();

	while ((i = q.ask()) != 1);

	redraw();
	_headDraw();
	_footDraw();


	while ((ch = getch()) != 10)
	{
		if(t.resize()) {
			redraw();
			_headDraw();
			_footDraw();
		}
		t.SetCursorPos(0,t.scr.x-2);
		if(ch == 27) k = 1;
		if(ch == 91 && k == 1) k = 2;
		if(ch == 65 && k == 2) setHint("Up Key");
		if(ch == 66 && k == 2) setHint("Down Key");
		if(ch == 67 && k == 2) setHint("Right Key");
		if(ch == 68 && k == 2) setHint("Left Key");
		if(ch != 27 && ch != 91) k = 0;
		if(ch == 10) setHint("Enter");
	}


	while (true) {
		t.SetCursorPos(0,t.scr.x-2);
		//s = getkey();
		s = getstr();
		if (!s.empty() ) {
			setHint(s);
			t.SetCursorPos(0,t.scr.x-2);
			for (int i = 0; i < s.length(); i++) {
				std::cout << " ";
			}
		}
	}
}

void game::setHint(std::string s) {
	t.SetCursorPos(0,1);
	if (s.length() >= t.scr.y-4) {
		s = s.substr(0, (t.scr.y-5	));
	}
	std::cout << " ≡ ";
	s.erase(s.size() - 1);
	std::cout << s;
	//std::cout << s.length();
	for (size_t r = s.length() + 4 ; r < t.scr.y; r++) {
		std::cout << " ";
	}
}

void game::redraw() {
	t.resize();
	_cleanHint();
	t.SetCursorPos(0,2);

	int i = (t.scr.y%2==1)?(1+(t.scr.y)/2):((t.scr.y)/2);

	int px = 4;

	if(t.scr.y >= 48) {
		for (int l = 0; l < 16; l++) {
			for (size_t r = 0; r < i-(4*px); r++) {
				std::cout << " ";
			}
			if(l%4<2) {
				for (int r = 0; r < 8*px; r++) {
					if (r%(2*px)<px) std::cout<<"▒";
					else std::cout<<"█";
				}
				//std::cout<<"▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████";
			} else {
				for (int r = 0; r < 8*px; r++) {
					if (r%(2*px)>=px) std::cout<<"▒";
					else std::cout<<"█";
				}
				//std::cout<<"█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒";
			}
			for (size_t r = 0; r < (t.scr.y)/2-(4*px); r++) {
				std::cout << " ";
			}
		}
		for (size_t r = 18; r < (t.scr.x -1); r++) {
			for (size_t r = 0; r < (t.scr.y); r++) {
				std::cout << " ";
			}
		}
	} else if(t.scr.y >= 24) {
		i = (t.scr.y%2==1)?(1+(t.scr.y)/2):((t.scr.y)/2);
		for (int l = 0; l < 8; l++) {
			for (size_t r = 0; r < i-8; r++) {
				std::cout << " ";
			}
			if(l%2==1) {
				for (int r=0; r<2*8;r++) {
					if (r%4<2) std::cout << "▒";
					else std::cout<<"█";
				}
			//	std::cout<<"▒▒██▒▒██▒▒██▒▒██";
			} else {
				for (int r=0; r<2*8;r++) {
					if (r%4>=2) std::cout << "▒";
					else std::cout<<"█";
				}
				//std::cout<<"██▒▒██▒▒██▒▒██▒▒";
			}
			for (size_t r = 0; r < (t.scr.y)/2-8; r++) {
				std::cout << " ";
			}
		}
		for (size_t r = 10; r < (t.scr.x -1); r++) {
			for (size_t r = 0; r < (t.scr.y); r++) {
				std::cout << " ";
			}
		}
	}

}

term::term() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	_height	= scr.x	=	w.ws_row;
	_width	= scr.y	=	w.ws_col;
}

void term::SetCursorPos(int x, int y) {
	printf("\033[%d;%dH", y+1, x+1);
}

bool term::resize() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	if (w.ws_row != scr.x) scr.x = w.ws_row;
	if (w.ws_col != scr.y) scr.y = w.ws_col;
	if(_height	== scr.x	&& _width	== scr.y) return false;
	else {
		_height=scr.x;
		_width=scr.y;
	}
	return true;
}


int *term::get() {
	int rtrn[2] = {_height,_width};
	return rtrn;
}

term::~term( ) {
	for (int y = 0; y < scr.x; y++) {
		for (int y = 0; y < scr.y; y++) {
			std::cout << " ";
		}
		std::cout << std::endl;
	}
	SetCursorPos(0, 0);
}

void initBoard() {
	//TODO:INIT

}

void etest() {
	std::cout <<"▀██████▀\n  ▀▀▀▀  \n  ▄▄▄▄	 \n▄██████▄\n";
	//▒ ▓ █ ▀ ▄ ■
}

void ech_redraw(int line) {

}

int main(void) {
	game g(true);
	//g.loop();
	return 0;
}

int mmain(void) {
	game g;


	std::cout<<"a\nb\nc\n";
	term t;

	std::string title = "debug is a very long title lol";

	std::cin >> title;

	int i;
	int j;

	i = t.scr.y;
	i -= ilen(t.scr.x);
	i -=1;
	i -= ilen(t.scr.y);
	i-=1;
	//std::cout << i << std::endl;
	t.SetCursorPos(0,0);
	std::cout << " ▀▄ ";
	for (int y = 4; y < t.scr.y; y++) {
		std::cout << "░";
	}
	//▒ ▓ █ ▀ ▄ ■
	t.SetCursorPos(i,0);
	std::cout << " " << t.scr.x << "-" << t.scr.y << std::endl;
	j = (t.scr.y)/2;
	j -= 4+title.length()/2;
	t.SetCursorPos(j,0);
	std::cout <<" == "<<title<<" == ";
	//std::cout <<" == scr == ";
	t.SetCursorPos(0,1);
	for (size_t r = 0; r < (t.scr.y); r++) {
		std::cout << " ";
	}
	t.SetCursorPos(0,2);

	i = (t.scr.y%2==1)?(1+(t.scr.y)/2):((t.scr.y)/2);

	if(t.scr.y >= 48) {
		for (int l = 0; l < 16; l++) {
			for (size_t r = 0; r < i-25; r++) {
				std::cout << " ";
			}
			if(l%4<2) {
				std::cout<<"▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████";
			} else {
				std::cout<<"█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒█████▒▒▒▒▒";
			}
			for (size_t r = 0; r < (t.scr.y)/2-25; r++) {
				std::cout << " ";
			}
		}
	} else if(t.scr.y >= 24) {
		i = (t.scr.y%2==1)?(1+(t.scr.y)/2):((t.scr.y)/2);
		for (int l = 0; l < 8; l++) {
			for (size_t r = 0; r < i-8; r++) {
				std::cout << " ";
			}
			if(l%2==1) {
				std::cout<<"▒▒██▒▒██▒▒██▒▒██";
			} else {
				std::cout<<"██▒▒██▒▒██▒▒██▒▒";
			}
			for (size_t r = 0; r < (t.scr.y)/2-8; r++) {
				std::cout << " ";
			}
		}
	}


	//t.SetCursorPos(2,1);
	//std::cout << "plop !" << std::endl;


	/*std::cout<<"lol                                          \nowo\nhey\n";
	printf ("lines %d\n", w.ws_row);
  printf ("columns %d\n", w.ws_col);*/
	return 0;
}
