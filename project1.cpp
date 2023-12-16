// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725
#include <vector>
#include <iostream>
#include <algorithm> // std::sort
#include <getopt.h>
#include <string>    // needed for VS
#include <deque>


using namespace std;

struct State {
	char color;
	size_t row;
	size_t column;
};


enum class Mode {
	kNone = 0,
	kStack,
	kQueue
};

struct Options {
	Mode mode = Mode::kNone;
};


class Puzzle {

public:

	void read_data();

	void get_options(int argc, char** argv, Options& option);

	void discovery(Options& options);

	void checkNorth(size_t x, size_t y, char color);
	void checkSouth(size_t x, size_t y, char color);
	void checkEast(size_t x, size_t y, char color);
	void checkWest(size_t x, size_t y, char color);

	void checkButton(int direction, size_t x, size_t y, char color);

	bool walkOn(size_t x, size_t y, char color);

	bool letterExists(size_t x, size_t y);

	bool checkNotDiscovered(size_t x, size_t y, char color);
	bool buttonDiscovered(size_t x, size_t y);
	void checkSolution(size_t x, size_t y, State stated);
	void printSolution(vector<State> &print);
	void getPath(vector<State>& print);
	void printNoSolution();

	bool solvable;
	Puzzle()
	{
		solvable = false;
		dequeOp = 0;
		numQuest = 0;
		numStart = 0;
		printOp = 0;
	}

private:

	vector<vector<vector<char>>> discovered;
	vector<vector<char>> maize;
	deque<State> queuey;
	vector<char> rows;
	State temp;
	State final;
	size_t width;
	size_t height;
	size_t numColors;
	int dequeOp;
	int numQuest;
	int numStart;
	int printOp;

	void  queueOrStack(Options& options)
	{
		if (options.mode == Mode::kStack)
		{

			temp = queuey.back();
			queuey.pop_back();
		}
		else if (options.mode == Mode::kQueue)
		{

			temp = queuey.front();
			queuey.pop_front();
		}
	}
	
};

int main(int argc, char** argv) {
	ios_base::sync_with_stdio(false);
	

	Puzzle puzzle;
	Options options;
	vector<State> print;
	// Read and process the command line options.
	puzzle.get_options(argc, argv, options);

	// Read in the provided file through stdin.
	puzzle.read_data();


	puzzle.discovery(options);
	// All done!
	if (puzzle.solvable)
	{
		puzzle.getPath(print);
		puzzle.printSolution(print);
	}
	return 0;
}


size_t charToSizeT(char color)

{
	//lowercase values: between 97 and 122
	//uppercase vals: between 65 and 89
	int colorLower;
	if (color >= 97 && color <= 122) {
		colorLower = color - 96;
	}
	else if (color >= 65 && color <= 90) {
		colorLower = color - 64;
	}
	//char colorLower = char(tolower(color));
	else if (color == '^')
		colorLower = 0;
	else {
		colorLower = 69;
	}
	return (static_cast<size_t>(colorLower));
}



// Read and process command line options.
void Puzzle::get_options(int argc, char* argv[], Options& options) {
	
	opterr = false;
	int option_index = 0;
	int choice;

	// Don't display getopt error messages about options

	// use getopt to find command line options
	option longOpts[] = { { "output", required_argument, nullptr, 'o' },
								{ "help", no_argument, nullptr, 'h' },
								{ "stack", no_argument, nullptr, 's'},
								{ "queue", no_argument, nullptr, 'q'},
								{ nullptr, 0, nullptr, '\0' } };

	//if output has no arg
	while ((choice = getopt_long(argc, argv, "o:hsq", longOpts, &option_index)) != -1) {
		switch (choice)
		{
			case 'o':
			{
				string arg{ optarg };
				if (arg == "map")
				{
					printOp = 1;
				}
				//figure out what to do if given both
				else if (arg == "list")
				{
					printOp = 2;
				}
				else
				{
					printOp = 3;
					cerr << "Error: Invalid output mode";
					exit(1);
				}
				break;
			}
			case 's':
				options.mode = Mode::kStack;
				dequeOp++;
				break;
			case 'q':
				options.mode = Mode::kQueue;
				dequeOp++;
				break;
			case 'h':
				std::cout << "This program reads in a puzzle that contains symbols,\n"
					<< "that one can solve to get out of the maize\n"
					<< " It then outputs the solution of getting out\n"
					<< "in the command line arguments (the output option), which\n"
					<< "defaults to a map, solved by either a stack or queue,\n"
					<< "Usage: \'./puzzle\n\t[--stack | -s]\n"
					<< "\t[--queue | -q]\n"
					<< "\t[--output | -o] <map or list>\n"
					<< "\t[--help | -h]\n"
					<< std::endl;
				exit(0);
			default:
				cerr << "Error: invalid option" << "\n";
				exit(1);
		}
	}
	if (dequeOp > 1)
	{
		cerr << "Error: Can not have both stack and queue";
		exit(1);
	}
	if (printOp == 0)
	{
		printOp = 1;
	}
	if (options.mode == Mode::kNone)
	{
		cerr << "Error: Must have at least one of stack or queue";
		exit(1);
	}
}

void Puzzle::read_data() {
	
	string junk;

	cin >> numColors >> height >> width;
	if (numColors > 26 /*numcolors < 0*/)
	{
		cerr << "Invalid numColor\n";
		exit(1);
	}
	if (height < 1)
	{
		cerr << "Invalid height\n";
		exit(1);
	}
	if (width < 1)
	{
		cerr << "Invalid width\n";
		exit(1);
	}
	if (width == 1 && height == 1)
	{
		cerr << "No\n";
		exit(1);
	}
	getline(cin, junk);
	maize.resize(height, vector<char>(width, ' '));
	discovered.resize(numColors + 1, vector<vector<char>>(height, vector<char>(width, '&')));
	string line;
	size_t i = 0;

	while (getline(cin, line))
	{

		if (!(line.empty() || line[0] == '/')) {
			for (size_t j = 0; j < width; ++j)
			{

				if (line[j] == '@')
				{
					temp.color = '^';
					temp.row = i;
					temp.column = j;
					queuey.push_back(temp);
					discovered[charToSizeT(temp.color)][i][j] = '@';
					numStart++;
				}
				else if (line[j] == '?')
				{
					numQuest++;
				}
				else if (line[j] != '^' && line[j] != '.' && line[j] != '#' && charToSizeT(line[j]) > numColors)
				{
					if (charToSizeT(line[j]) > 26)
					{
						cerr << "Error: Invalid character in map\n";
						exit(1);
					}
					else
					{
						cerr << "Error: Invalid door/button in map\n";
						exit(1);
					}
				}
				maize[i][j] = line[j];
			}
			i++;
		}
		
	}
	if (numStart > 1 || numQuest > 1)
	{
		cerr << "Error: Puzzle must have only one start and one target";
		exit(1);
	}
}



void Puzzle::discovery(Options& options)
{
	
	while (!queuey.empty() && !solvable)
	{
		queueOrStack(options);
		size_t x = temp.row;
		size_t y = temp.column;
		char color = temp.color;
		//if '^' after first one, blocks all colors again-> inactive at start
		//active after

		//check to see if it reached end
		//must have just one
		//check to see if its a button
		if (letterExists(x, y) && color != maize[x][y])
		{
			if (buttonDiscovered(x, y))
			{
				State button;
				button.row = x;
				button.column = y;
				button.color = maize[x][y];
				queuey.push_back(button);
				discovered[charToSizeT(maize[x][y])][x][y] = color;
			}
		}
		else
		{
			//these check if its valid to go there
			//then if it has been discovered
			//then if its something that could be walked on
			
			checkNorth(x, y, color);
			checkEast(x, y, color);
			checkSouth(x, y, color);
			checkWest(x, y, color);
		}
	}

	if (!solvable)
	{
		cout << "No solution.";
		printNoSolution();
	}
}


void Puzzle::checkNorth(size_t x, size_t y, char color)
{
	if (x > 0)
	{
		if (checkNotDiscovered(x - 1, y, color))
		{
			checkButton(1, x-1, y, color);
		}
	}
}
void Puzzle::checkSouth(size_t x, size_t y, char color)
{
	if (x < height - 1)
	{
		if (checkNotDiscovered(x + 1, y, color))
		{
			checkButton(2, x+1, y, color);
		}

	}
}
void Puzzle::checkEast(size_t x, size_t y, char color)
{
	if (y < width - 1)
	{
		if (checkNotDiscovered(x, y + 1, color))
		{
			checkButton(4, x, y+1, color);
		}
	}
}
void Puzzle::checkWest(size_t x, size_t y, char color)
{
	if (y > 0)
	{
		if (checkNotDiscovered(x, y - 1, color))
		{
			checkButton(3, x, y-1, color);
		}
	}
}

void Puzzle::checkButton(int direction, size_t x, size_t y, char color)
{
	State stated;
	
	if (direction == 1) // south
	{

		//something i can walk on
		if (walkOn(x, y, color))
		{
			discovered[charToSizeT(color)][x][y] = 'S';
			stated.row = x;
			stated.column = y;
			stated.color = color;
			queuey.push_back(stated);
			checkSolution(x, y, stated);
		}
	}
	else if (direction == 2) // north
	{
		if (walkOn(x, y, color))
		{
			// maize[x+1][y] != '#'
			discovered[charToSizeT(color)][x][y] = 'N';
			stated.row = x;
			stated.column = y;
			stated.color = color;;
			queuey.push_back(stated);
			checkSolution(x, y, stated);
		}
	}
	else if (direction == 3) // east
	{
		if (walkOn(x, y, color))
		{
			// maize[x][y-1] != '#'
			discovered[charToSizeT(color)][x][y] = 'E';
			stated.row = x;
			stated.column = y;
			stated.color = color;
			queuey.push_back(stated);
			checkSolution(x, y, stated);
		}
	}
	else // west
	{
		if (walkOn(x, y, color))
		{
			//maize[x][y+1] != '#'
			discovered[charToSizeT(color)][x][y] = 'W';
			stated.row = x;
			stated.column = y;
			stated.color = color;
			queuey.push_back(stated);
			checkSolution(x, y, stated);
		}
	}
	
}

void Puzzle::checkSolution(size_t x, size_t y, State stated)
{
	if (maize[x][y] == '?')
	{
		final = stated;
		solvable = true;
		//printSolution();
	}
}

bool Puzzle::letterExists(size_t x, size_t y)
{	
	char c = maize[x][y];
	return (isalpha(c) && islower(c)) || c == '^'; 
}

bool Puzzle::walkOn(size_t x, size_t y, char color)
{
	char c = maize[x][y];
	if(isalpha(c) && islower(c)){
		return true;
	}
	if(c == '.' || c == '?' || c == '^' || c == '@'){
		return true;
	}
	// can I walk on a wall
	if(isalpha(c) && isupper(c) && toupper(color) == c){
		return true;
	}
	return false;
}

bool Puzzle::checkNotDiscovered(size_t x, size_t y, char color)
{
	return discovered[charToSizeT(color)][x][y] == '&';
}

bool Puzzle::buttonDiscovered(size_t x, size_t y)
{
	return discovered[charToSizeT(maize[x][y])][x][y] == '&';
}


char sizeTToChar(size_t color)
{
	if (color == 0)
	{
		return '^';
	}
	return static_cast<char>('a' + color - 1);
}


void Puzzle::getPath(vector<State> &print)
{
	State location = final;
	while (discovered[charToSizeT(location.color)][location.row][location.column] != '@')
	{
		print.push_back(location);
		if (discovered[charToSizeT(location.color)][location.row][location.column] == 'N')
		{
			location.row = location.row - 1;
		}
		else if (discovered[charToSizeT(location.color)][location.row][location.column] == 'S')
		{
			location.row = location.row + 1;
		}
		else if (discovered[charToSizeT(location.color)][location.row][location.column] == 'E')
		{
			location.column = location.column + 1;
		}
		else if (discovered[charToSizeT(location.color)][location.row][location.column] == 'W')
		{
			location.column = location.column - 1;
		}
		else if (charToSizeT(discovered[charToSizeT(location.color)][location.row][location.column]) < numColors + 1)
		{
			location.color = discovered[charToSizeT(location.color)][location.row][location.column];
		}

	}
	print.push_back(location);
	location = final;
	discovered[charToSizeT(location.color)][location.row][location.column] = '?';

}

void Puzzle::printSolution(vector<State> &print)
{
	/*You’re right that you should NOT iterate through 
	the whole 3D vector and search for the places you’ve been. Instead, take it the other way 
	around and pop each item out of your Solution Path container, using each solution path location 
	to “trace" your solution path somewhere in a 3d vector (hint: use the 3d vector you don’t need anymore.)
	you can then print out each layer of the map mostly from your 2d map, but checking for when your current
	location is represented by ‘+’ or other solution path characters in backtrace (because you already traced 
	your solution path there).*/
	if (printOp == 1)
	{
		for (size_t i = 0; i < print.size() - 1; ++i)
		{
			if (print[i].color != print[i + 1].color && print[i].row == print[i + 1].row && print[i].column == print[i + 1].column)
			{
				discovered[charToSizeT(print[i].color)][print[i].row][print[i].column] = '@';
				discovered[charToSizeT(print[i+1].color)][print[i].row][print[i].column] = '%';
			}
				else if (discovered[charToSizeT(print[i].color)][print[i].row][print[i].column] != '%' && 
					discovered[charToSizeT(print[i].color)][print[i].row][print[i].column] != '@' && 
					discovered[charToSizeT(print[i].color)][print[i].row][print[i].column] != '?') {
					discovered[charToSizeT(print[i].color)][print[i].row][print[i].column] = '+';
				}
		}

		for (size_t i = 0; i < numColors + 1; ++i)
		{
			cout << "// color " << sizeTToChar(i) << "\n";
			for (size_t j = 0; j < height; ++j)
			{
				for (size_t k = 0; k < width; ++k)
				{
					
					if (discovered[i][j][k] != '+' && discovered[i][j][k] != '%' && discovered[i][j][k] != '@')
					{
						discovered[i][j][k] = maize[j][k];
						if (maize[j][k] == '@' && i == 0)
						{
							discovered[i][j][k] = '@';
						}
						if ((maize[j][k] == '@' && i > 0) || i == charToSizeT(maize[j][k]))
						{
							discovered[i][j][k] = '.';
						}
					}
					cout << discovered[i][j][k];
				}
				cout << '\n';
			}
		}
	}
	else if (printOp == 2)
	{
		while (!print.empty())
		{
			cout << "(" << print.back().color << ", (" << print.back().row << ", " << print.back().column << "))\n";
			print.pop_back();
		}
	}
}

void Puzzle::printNoSolution()
{
	cout << "\nDiscovered:\n";

	//if that spot was in an of the backtrace
	//if not if true 
	//not true # 
	//loop through map
	//then loop through backtrace
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			int undiscovered = 0;
			for (size_t i = 0; i < numColors + 1; ++i)
			{
				if (discovered[i][y][x] != '&')
					undiscovered++;
			}
			if (undiscovered == 0)
			{
				maize[y][x] = '#';
			}
			cout << maize[y][x];
		}
		cout << "\n";
	}
}