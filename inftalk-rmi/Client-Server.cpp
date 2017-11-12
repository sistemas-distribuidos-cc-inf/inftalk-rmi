#include <iostream>
#include <time.h>
#include <RCF/RCF.hpp>
#include <SF/string.hpp>
#include <SF/map.hpp>
#define TMap std::map
struct FBullCowCount
{
	int BullCount = 0;
	int CowCounts = 0;
	void serialize(SF::Archive &ar)
	{
		ar & BullCount & CowCounts;
	}
};

enum EGuessStatus
{
	Invalid_Status,
	OK,
	Not_Isogram,
	Wrong_Length,
	Not_Lowercase
};

RCF_BEGIN(I_BullCowGame, "I_BullCowGame")
RCF_METHOD_V0(void, reset)
RCF_METHOD_R0(int, GetMaxTries)
RCF_METHOD_R0(int, GetCurrentTries)
RCF_METHOD_R0(std::string, GetMyHiddenWord)
RCF_METHOD_R0(int, GetMyHiddenWordLength)
RCF_METHOD_R0(bool, IsGameWon)
RCF_METHOD_R0(std::string, GetWordInDictionary)
RCF_METHOD_R1(EGuessStatus, CheckGuessValidity, std::string)
RCF_METHOD_R1(FBullCowCount, SubmitValidGuess, std::string)
RCF_METHOD_R1(bool, isLowerCase, std::string)
RCF_METHOD_R1(bool, isIsogram, std::string)
RCF_METHOD_V0(void, PrintIntro) // Print the introduction of the game
RCF_METHOD_V0(void,PlayGame) // Começa o jogo
RCF_METHOD_R0(std::string,GetValidGuess) //Pega a resposta via teclado
RCF_METHOD_R0(bool,AskToPlayAgain) // Pergunta se Deseja continuar jogando
RCF_END(I_BullCowGame)

class BullCowGameImpl
{
private:
	int MyCurrentTry;
	int MyMaxTries;
	std::string MyHiddenWord;
	bool bGameIsWon;

public:

	BullCowGameImpl::BullCowGameImpl()
	{
		reset();
	}

	int GetCurrentTries()
	{
		return MyCurrentTry;
	}

	bool isLowerCase(std::string Guess)
	{
		for (auto Letter : Guess)
		{
			if (!islower(Letter))
			{
				return false;
			}
		}
		return true;
	}
	bool isIsogram(std::string Guess)
	{
		if (Guess.length() <= 1)
		{
			return true;
		}

		TMap <char, bool> LetterSeen;

		for (auto Letter : Guess)
		{
			Letter = tolower(Letter);
			if (LetterSeen[Letter])
			{
				return false;
			}
			else
			{
				LetterSeen[Letter] = true;
			}
		}
		return true;
	}

	std::string GetMyHiddenWord()
	{
		return MyHiddenWord;
	}

	int GetMyHiddenWordLength()
	{
		return MyHiddenWord.length();
	}

	int GetMaxTries()
	{
		TMap<int, int> WordLengthToMaxTries{ {4,8},{5,9},{6,10},{7,12},{8,15} };

		return WordLengthToMaxTries[GetMyHiddenWordLength()];
	}


	void reset()
	{

		MyHiddenWord = GetWordInDictionary();
		MyCurrentTry = 1;
		bGameIsWon = false;
		return;
	}

	bool IsGameWon()
	{
		return bGameIsWon;
	}

	std::string GetWordInDictionary()
	{
		int random = 0;
		srand(time(0));
		random = rand() % 19;
		TMap<int, std::string>WordInDictionary
		{
		{0,"drive"},
		{	1,"python" },
		{	2,"mouse"},
		{	3,"tela"},
		{	4,"tecla"},
		{	5,"site"},
		{	6,"cabo"},
		{	7,"cafe"},
		{	8,"github"},
		{	9,"blog"},
		{	10,"chat"},
		{	11,"chip"},
		{	12,"email"},
		{	13,"hacker"},
		{	14,"host"},
		{	15,"link"},
		{	16,"software"},
		{	17,"virus"},
		{18,"teclado"}
		};
		return WordInDictionary[random];
	}

	EGuessStatus CheckGuessValidity(std::string Guess)
	{
		if (!isIsogram(Guess))
		{
			return EGuessStatus::Not_Isogram;
		}
		else if (!isLowerCase(Guess))
		{
			return EGuessStatus::Not_Lowercase;
		}
		else if (Guess.length() != GetMyHiddenWordLength())
		{
			return EGuessStatus::Wrong_Length;
		}
		else
		{
			return EGuessStatus::OK;
		}

	}

	FBullCowCount SubmitValidGuess(std::string Guess)
	{

		MyCurrentTry++;
		FBullCowCount BullCowCount;

		int WordLenght = MyHiddenWord.length();

		for (int MHWChar = 0; MHWChar < WordLenght; MHWChar++)
		{
			for (int GChar = 0; GChar < WordLenght; GChar++)
			{
				if (Guess[GChar] == MyHiddenWord[MHWChar])
				{
					if (MHWChar == GChar)
						BullCowCount.BullCount++;
					else
						BullCowCount.CowCounts++;
				}

			}
		}
		if (BullCowCount.BullCount == WordLenght)
		{
			bGameIsWon = true;
		}
		else
		{
			bGameIsWon = false;
		}
		return BullCowCount;
	}

	void PlayGame()
	{

		int MaxTries = GetMaxTries();
		reset();

		while (!IsGameWon() && GetCurrentTries() <= MaxTries)
		{
			std::string Word = GetValidGuess();
			FBullCowCount BullCowCount = SubmitValidGuess(Word);
			std::cout << "Bulls = " << BullCowCount.BullCount;
			std::cout << ". Cows = " << BullCowCount.CowCounts << std::endl;
			if (IsGameWon())
			{
				std::cout << "\n\n* * * * P A R A B E N S --- V O C E   D E S C O B R I U  A  P A L A V R A   S E C R E T A  * * * *\n\n" << std::endl;
			}
			else
			{
				if (GetCurrentTries() > 10)
					std::cout << "\n\n* * * * V O C E   P E R D E U * * * *\n\n" << std::endl;
			}
		}
	}

	void PrintIntro()
	{
		std::string Word = GetMyHiddenWord();

		std::cout << "Bem-Vindo ao jogo Bull and Cows\n" << std::endl;
		std::cout << "                 }   {         ___ " << std::endl;
		std::cout << "                 (o o)        (o o) " << std::endl;
		std::cout << "          /-------\\ /          \\ /-------\\		 " << std::endl;
		std::cout << "         / | BULL |O            O| COW  | \\			" << std::endl;
		std::cout << "        *  |-,--- |              |------|  *			" << std::endl;
		std::cout << " _\\|/_    ^      ^     _\\|/_    ^      ^    _\\|/_\n" << std::endl;
		std::cout << "Voce tem " << GetMaxTries() << " tentativas para conseguir descobrir a palavra secreta\n " << std::endl;
		std::cout << "* * * D I C A * * *: contem " << Word.length() << " letras nao repetidas\n" << std::endl;

		std::cout << "Insira a palavra que voce acha ser a resposta: " << std::endl;
		return;

	}


	std::string GetValidGuess()
	{
		EGuessStatus Status = EGuessStatus::Invalid_Status;
		std::string Guess = "";
		do {

			std::getline(std::cin, Guess);
			std::cout << "\nTentativa " << GetCurrentTries() << " de " << GetMaxTries() << " : " << Guess << std::endl;
			Status = CheckGuessValidity(Guess);
			switch (Status)
			{
			case EGuessStatus::OK:
				break;
			case EGuessStatus::Not_Isogram:
				std::cout << "Erro, Por Favor digite uma palavra sem letras repetidas\n " << std::endl;
				break;
			case EGuessStatus::Wrong_Length:
				std::cout << "Erro, Por Favor digite uma palavra de " << GetMyHiddenWordLength() << " letras\n" << std::endl;
				break;
			case EGuessStatus::Not_Lowercase:
				std::cout << "Erro, Por Favor digite uma palavra sem letras maiusculas\n " << std::endl;
				break;
			default:
				break;
			}std::cout << std::endl;
		} while (Status != EGuessStatus::OK);
		return Guess;
	}

	bool AskToPlayAgain()
	{
		std::cout << "Deseja continuar jogando ? (s/n) " << std::endl;
		std::string Resposta;
		std::getline(std::cin, Resposta);
		return (Resposta[0] == 's') || (Resposta[0] == 'S');
	}
};

int main()
{
	RCF::RcfInitDeinit rcfInit;

	BullCowGameImpl BullCowGame;
	RCF::RcfServer server(RCF::TcpEndpoint(50001));

	server.bind<I_BullCowGame>(BullCowGame);

	server.start();

	std::cout << "Calling the I_BullCowGame PlayGame() method." << std::endl;

	RcfClient<I_BullCowGame> client(RCF::TcpEndpoint(50001));
	//client.PrintIntro();
	client.PlayGame();
	//client.AskToPlayAgain();
	system("PAUSE");
	return 0;
}