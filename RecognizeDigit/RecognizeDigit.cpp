#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <vector>

const unsigned BOK = 28;          
const float SKALA = 20.0f;       
const float GRUBOSC = 1.5f;      

std::vector<float> piksele(BOK* BOK, 0.0f);

void maluj(sf::Vector2f srodek)
{
    for (int y = 0; y < int(BOK); y++)
        for (int x = 0; x < int(BOK); x++) {
            const float odl = std::hypot(x + 0.5f - srodek.x, y + 0.5f - srodek.y);
            const float jasnosc = std::clamp(GRUBOSC - odl, 0.0f, 1.0f);
            float& p = piksele[y * BOK + x];
            p = std::max(p, jasnosc);
        }
}

void linia(sf::Vector2f a, sf::Vector2f b)
{
    const sf::Vector2f roznica = b - a;
    const float dlugosc = std::hypot(roznica.x, roznica.y);
    const int kroki = std::max(1, int(dlugosc / 0.1f));
    for (int i = 0; i <= kroki; i++)
        maluj(a + roznica * (float(i) / kroki));
}

int main()
{
    std::cout << "Sterowanie w oknie:\n";
    std::cout << "  lewy przycisk myszy - rysowanie\n";
    std::cout << "  C                   - wyczyszczenie\n";
    std::cout << "  Esc                 - wyjscie\n";

    const unsigned rozmiarOkna = unsigned(BOK * SKALA);
    sf::RenderWindow okno(sf::VideoMode({ rozmiarOkna, rozmiarOkna }), "Rysowanie",
        sf::Style::Titlebar | sf::Style::Close);
    okno.setFramerateLimit(60);

    const sf::Vector2u rozmiar(BOK, BOK);
    sf::Image obraz(rozmiar, sf::Color::Black);
    sf::Texture tekstura(rozmiar);  
    sf::Sprite sprite(tekstura);
    sprite.setScale({ SKALA, SKALA });

    std::optional<sf::Vector2f> ostatniaPozycja;

    while (okno.isOpen()) {
        while (const std::optional zdarzenie = okno.pollEvent()) {
            if (zdarzenie->is<sf::Event::Closed>())
                okno.close();
            else if (const auto* klawisz = zdarzenie->getIf<sf::Event::KeyPressed>()) {
                if (klawisz->code == sf::Keyboard::Key::Escape)
                    okno.close();
                if (klawisz->code == sf::Keyboard::Key::C)
                    std::fill(piksele.begin(), piksele.end(), 0.0f);
            }
        }

        if (okno.hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            const sf::Vector2f mysz = okno.mapPixelToCoords(sf::Mouse::getPosition(okno));
            const sf::Vector2f p = mysz / SKALA;
            linia(ostatniaPozycja.value_or(p), p);
            ostatniaPozycja = p;
        }
        else {
            ostatniaPozycja.reset();
        }

        for (unsigned y = 0; y < BOK; y++) {
            for (unsigned x = 0; x < BOK; x++) {
                const auto j = std::uint8_t(piksele[y * BOK + x] * 255.0f);
                obraz.setPixel({ x, y }, sf::Color(j, j, j));
            }
        }
        tekstura.update(obraz);

        okno.clear();
        okno.draw(sprite);
        okno.display();
    }
}