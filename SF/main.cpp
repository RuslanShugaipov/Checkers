#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

int game_field[8][8] =
{
	2,0,2,0,2,0,2,0,
	0,2,0,2,0,2,0,2,
	2,0,2,0,2,0,2,0,
	0,2,0,2,0,2,0,2,
	2,0,2,0,2,0,2,0,
	0,2,0,2,0,2,0,2,
	2,0,2,0,2,0,2,0,
	0,2,0,2,0,2,0,2,
};

enum Disc_Color
{
	BLACK,
	WHITE
};

class Disc
{
public:
	Disc() : color(Disc_Color::WHITE), position_x(0), position_y(0), is_selected_(false), is_pointed_(false) {};
	Disc(Disc_Color color, int x, int y) : color(color), position_x(x), position_y(y), is_selected_(false), is_pointed_(false) { game_field[x][y] = 1; }

	int get_x() const { return position_x; }
	int get_y() const { return position_y; }

	bool is_selected() const { return is_selected_; }
	bool is_pointed() const { return is_pointed_; }

	void set_is_selected(bool flag) { is_selected_ = flag; }
	void set_is_pointed(bool flag) { is_pointed_ = flag; }

	Disc_Color get_color() const { return color; }

	void move(int x, int y)
	{
		if (game_field[y][x] == 0 && ((((position_x - y == -1 && position_y - x == -1) || (position_x - y == -1 && position_y - x == 1)) && color == Disc_Color::BLACK) ||
			(((position_x - y == 1 && position_y - x == 1) || (position_x - y == 1 && position_y - x == -1)) && color == Disc_Color::WHITE)))
		{
			game_field[position_x][position_y] = 0;
			game_field[y][x] = 1;
			position_x = y;
			position_y = x;
		}
	}

private:
	Disc_Color color;
	int position_x, position_y;
	bool is_selected_;
	bool is_pointed_;
};

class Render
{
public:
	Render()
	{
		window.create(sf::VideoMode(480, 480), "CHECKERS");

		//Текстура шашки
		texture_disc.loadFromFile("images/discs.jpg");
		texture_disc_pointed.loadFromFile("images/discs_pointed.jpg");
		texture_disc_selected.loadFromFile("images/discs_selected.jpg");

		//Текстура ячейки игрового поля, на которую установлен курсор
		texture_cell_pointed.loadFromFile("images/cell_pointed.jpg");

		//Текстура задника
		texture_background.loadFromFile("images/checkers.jpg");

		//Спрайт шашки
		sprite_disc.setTexture(texture_disc);
		sprite_disc_pointed.setTexture(texture_disc_pointed);
		sprite_disc_selected.setTexture(texture_disc_selected);

		//Спрайт ячейки игрового поля, на которую установлен курсор
		sprite_cell_pointed.setTexture(texture_cell_pointed);

		//Спрайт задника
		sprite_background.setTexture(texture_background);
	}

	void draw_disc(const Disc& disc)
	{
		if (disc.is_pointed())
		{
			sprite_disc_pointed.setTextureRect(sf::IntRect(disc.get_color() * 60, 0, 60, 60));
			sprite_disc_pointed.setPosition(disc.get_y() * 60, disc.get_x() * 60);
			window.draw(sprite_disc_pointed);
		}
		else if (disc.is_selected())
		{
			sprite_disc_selected.setTextureRect(sf::IntRect(disc.get_color() * 60, 0, 60, 60));
			sprite_disc_selected.setPosition(disc.get_y() * 60, disc.get_x() * 60);
			window.draw(sprite_disc_selected);
		}
		else
		{
			sprite_disc.setTextureRect(sf::IntRect(disc.get_color() * 60, 0, 60, 60));
			sprite_disc.setPosition(disc.get_y() * 60, disc.get_x() * 60);
			window.draw(sprite_disc);
		}
	}

	void draw(int x, int y, sf::Sprite sprite)
	{
		sprite.setPosition(x * 60, y * 60);
		window.draw(sprite);
	}

	sf::RenderWindow window;
	sf::Texture texture_disc, texture_disc_selected, texture_disc_pointed;
	sf::Texture texture_cell_pointed;
	sf::Texture texture_background;
	sf::Sprite sprite_disc, sprite_disc_selected, sprite_disc_pointed;
	sf::Sprite sprite_cell_pointed;
	sf::Sprite sprite_background;
};

class Cursor
{
public:
	static Cursor& Instance()
	{
		static Cursor instance;
		return instance;
	}

	Cursor(const Cursor&) = delete;

	int get_x() const { return position_x; }
	int get_y() const { return position_y; }

	void move(int x, int y)
	{
		if (position_x + x <= 7 && position_x + x >= 0)
			position_x += x;
		if (position_y + y <= 7 && position_y + y >= 0)
			position_y += y;
	}

private:
	Cursor() : position_x(0), position_y(0) {};

	int position_x, position_y;
};

void begin_game(std::vector<Disc>& black_discs, std::vector<Disc>& white_discs)
{
	for (int i = 0; i < 4; ++i)
	{
		black_discs.push_back(Disc(Disc_Color::BLACK, 0, 2 * i + 1));
		black_discs.push_back(Disc(Disc_Color::BLACK, 1, 2 * i));
		black_discs.push_back(Disc(Disc_Color::BLACK, 2, 2 * i + 1));

		white_discs.push_back(Disc(Disc_Color::WHITE, 5, 2 * i));
		white_discs.push_back(Disc(Disc_Color::WHITE, 6, 2 * i + 1));
		white_discs.push_back(Disc(Disc_Color::WHITE, 7, 2 * i));
	}
}

int main()
{
	std::vector<Disc> black_discs;
	std::vector<Disc> white_discs;

	Render render;

	Cursor& cursor = Cursor::Instance();

	//Итераторы, указывающие на черную и белую шашки, на которые установлен курсор
	std::vector<Disc>::iterator white_disc_pointed, black_disc_pointed;

	std::vector<Disc>::iterator white_disc_selected, black_disc_selected;

	//Переменная, которая нужна для инициализации игры, потом она равна false
	bool is_begin_game = true;

	while (render.window.isOpen())
	{
		sf::Event event;
		while (render.window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				render.window.close();
			if (event.type == sf::Event::EventType::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Right)
					cursor.move(1, 0);
				if (event.key.code == sf::Keyboard::Left)
					cursor.move(-1, 0);
				if (event.key.code == sf::Keyboard::Up)
					cursor.move(0, -1);
				if (event.key.code == sf::Keyboard::Down)
					cursor.move(0, 1);
				if (event.key.code == sf::Keyboard::Escape)
					render.window.close();
			}
		}

		render.window.clear(sf::Color::White);

		//Отрисовка задника
		render.window.draw(render.sprite_background);

		//Обработка курсора
		if (game_field[cursor.get_y()][cursor.get_x()] == 2 || game_field[cursor.get_y()][cursor.get_x()] == 0)
		{
			render.draw(cursor.get_x(), cursor.get_y(), render.sprite_cell_pointed);
		}
		else if (game_field[cursor.get_y()][cursor.get_x()] == 1)
		{
			//Если курсор установлен на черную шашку
			black_disc_pointed = std::find_if(std::begin(black_discs), std::end(black_discs), [&cursor](const Disc& disc)
				{
					return (disc.get_x() == cursor.get_y()) && (disc.get_y() == cursor.get_x());
				});
			if (black_disc_pointed != black_discs.end())
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				{
					if (black_disc_selected != black_discs.end())
					{
						(*black_disc_selected).set_is_selected(false);
						if (white_disc_selected != white_discs.end())
							(*white_disc_selected).set_is_selected(false);
					}
					(*black_disc_pointed).set_is_selected(true);
					black_disc_selected = black_disc_pointed;
				}
				else
					(*black_disc_pointed).set_is_pointed(true);
				render.draw_disc(*black_disc_pointed);
			}

			//Если курсор установлен на белую шашку
			white_disc_pointed = std::find_if(std::begin(white_discs), std::end(white_discs), [&cursor](const Disc& disc)
				{
					return (disc.get_x() == cursor.get_y()) && (disc.get_y() == cursor.get_x());
				});
			if (white_disc_pointed != white_discs.end())
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				{
					if (white_disc_selected != white_discs.end())
					{
						(*white_disc_selected).set_is_selected(false);
						if (black_disc_selected != black_discs.end())
							(*black_disc_selected).set_is_selected(false);
					}
					(*white_disc_pointed).set_is_selected(true);
					white_disc_selected = white_disc_pointed;
				}
				else
					(*white_disc_pointed).set_is_pointed(true);
				render.draw_disc(*white_disc_pointed);
			}
		}

		//Начальная расстановка шашек
		if (is_begin_game)
		{
			begin_game(black_discs, white_discs);
			black_disc_pointed = black_discs.end();
			black_disc_selected = black_discs.end();
			white_disc_pointed = white_discs.end();
			white_disc_selected = white_discs.end();
			is_begin_game = false;
		}

		//Отображение черных шашек 
		for (auto& disc : black_discs)
		{
			render.draw_disc(disc);
		}
		//Отображение белых шашек
		for (auto& disc : white_discs)
		{
			render.draw_disc(disc);
		}

		//Если курсор больше не указывает на черную шашку
		if (black_disc_pointed != black_discs.end())
		{
			if ((*black_disc_pointed).get_x() != cursor.get_x() && (*black_disc_pointed).get_y() != cursor.get_y())
				(*black_disc_pointed).set_is_pointed(false);
		}

		//Если курсор больше не указывает на белую шашку
		if (white_disc_pointed != white_discs.end())
		{
			if ((*white_disc_pointed).get_x() != cursor.get_x() && (*white_disc_pointed).get_y() != cursor.get_y())
				(*white_disc_pointed).set_is_pointed(false);
		}

		//Движение шашки
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			if (black_disc_selected != black_discs.end())
			{
				(*black_disc_selected).set_is_selected(false);
				(*black_disc_selected).move(cursor.get_x(), cursor.get_y());
			}
			else if (white_disc_selected != white_discs.end())
			{
				(*white_disc_selected).set_is_selected(false);
				(*white_disc_selected).move(cursor.get_x(), cursor.get_y());
			}
		}

		render.window.display();
	}

	return 0;
}