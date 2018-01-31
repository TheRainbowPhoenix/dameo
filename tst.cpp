#include <sys/ioctl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <termios.h>

#define KB_K_UP					128	// cursor key Up
#define KB_K_DOWN		129	// cursor key Down
#define KB_K_RIGHT		130	// cursor key Right
#define KB_K_LEFT			131	// cursor key Left


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
		int *get();
		term();
		~term();
};

struct button {
	int id;
	std::string caption;
	bool cliquable;
};

class message {
	private:
		term _t;
	public:
		std::string title;
		button * buttons;
		std::string label;
		void show();
		message();
		message(term t);
		message(term t, std::string label, int type);
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

message::message() {
	term _t;
	title = "Message";
	button ok;
	ok.id = 1;
	ok.caption = "Ok";
	ok.cliquable = true;
	buttons = new button[3];
	buttons[0] = ok;
	buttons[1] = ok;
	buttons[2] = ok;
	label = "Oops something is missing ...";
}

message::message(term t) {
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

message::message(term t, std::string lbl, int type) {
	_t = t;
	button ok;
	ok.id = 1;
	ok.caption = "Ok";
	ok.cliquable = true;
	button yes;
	ok.id = 2;
	ok.caption = "Yes";
	ok.cliquable = true;
	button no;
	ok.id = 3;
	ok.caption = "No";
	ok.cliquable = true;
	
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
		}				

}


void message::show() {	
	int i;
	int j;
	int k;

	i = _t.scr.x/2 - (1+3+2)/2;
	j = _t.scr.y/4;

	if(_t.scr.y >= 48) {
		_t.SetCursorPos(j,i);
		for (int l = 0; l < _t.scr.y/4 - title.length()/2-1; l++) std::cout << "░";
		//_t.SetCursorPos(j,(_t.scr.y)/2);
		std::cout <<" "<<title<<" ";
		for (int l = 0; l < _t.scr.y/4 - title.length()/2-1; l++) std::cout << "░";
		_t.SetCursorPos(j,++i);
		printf("\033[7m");
		for (int l = 0; l < _t.scr.y/2; l++) std::cout << " ";
		_t.SetCursorPos(j,++i);	
		std::cout <<  " " << label;
		for (int y = label.length()+1; y < (_t.scr.y/2); y++) std::cout << " ";	
		_t.SetCursorPos(j,++i);
		for (int l = 0; l < _t.scr.y/2; l++) std::cout << " ";
		//for (int l = 0; l < _t.scr.y/2; l++) std::cout << " ";
		_t.SetCursorPos(j,++i);	
		k=(_t.scr.y/2 - buttons[0].caption.length())/2-2;
		for (size_t l = 0; l <k; l++)	std::cout << " ";
		printf("\033[27m");
		std::cout <<  "[ " << buttons[0].caption << " ]";
		printf("\033[7m");
		for (size_t l = k+buttons[0].caption.length()+4; l <_t.scr.y/2; l++)	std::cout << " ";
		_t.SetCursorPos(j,++i);	
		for (int l = 0; l < _t.scr.y/2; l++) std::cout << " ";
		printf("\033[27m");
	}
	_t.SetCursorPos(0,_t.scr.x-2);
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
	_headDraw();
	_footDraw();
	//t.SetCursorPos(0,t.scr.y);
	loop();
}

void game::_headDraw() {

	if(debug) {
		title.append(" [Debug]");
	}
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
	char buf[254];
	std::string s;
	message m(t, "Lol i'm a message :D", 1);
	m.show();
		//redraw();

	while ((ch = getch()) != 10)
	{
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
