/**
 * @file SfmlPolygon.hpp
 * @author Perry Chouteau (perry.chouteau@outlook.com)
 * @brief
 * @date 2026-08-10
 *
 * @addtogroup sfml
 * @{
 */

#ifndef SFMLPOLYGON_HPP_
#define SFMLPOLYGON_HPP_

#include <vector>

//Sfml
#include <SFML/Graphics.hpp>

// Interface
#include "IPolygon.hpp"

// system : Triangle, Vector2f, Epsilon
#include "Shape.hpp"

class SfmlPolygon : public graphic::IPolygon {

public:
    SfmlPolygon(std::vector<Vector2f> points) {
        _color = sf::Color{255, 0, 0, 255};
        _position = {0, 0};

        for (auto point : points) {
            _points.push_back(Vector2f{point.x, point.y});
        }

        triangulate();
        buildVertices();   // une seule fois : le draw n'a plus qu'a envoyer
    }

    ~SfmlPolygon() = default;

    bool isReady() const override {
        return true;
    }

    Vector2f getPosition() const override {
        return {_position.x, _position.y};
    }

    // position goes through a transform at draw time, so the vertices
    // never have to be rebuilt
    void setPosition(Vector2f position) override {
        _position = sf::Vector2f{float(position.x), float(position.y)};
    }

    Color getColor() const override {
        return Color{_color.r, _color.g, _color.b, _color.a};
    }

    void setColor(Color color) override {
        _color = sf::Color{color.r, color.g, color.b, color.a};
        for (size_t i = 0; i < _vertices.getVertexCount(); i++)
            _vertices[i].color = _color;
    }

    std::vector<Vector2f> getPoints() const override {
        return _points;
    }

    friend class SfmlWindow;

private:
    /**
     * @brief L'angle en B est-il saillant, pour un contour d'aire positive ?
     *
     * Le produit vectoriel de BA par BC est negatif sur un sommet convexe.
     * Nul veut dire trois points alignes : pas d'oreille a couper la.
     *
     * Ecrit ici plutot qu'importe : c'est cinq lignes, et ca evite de faire
     * dependre le vendor d'un repo de geometrie pour elles.
     */
    static bool is_convex(Vector2f A, Vector2f B, Vector2f C) {
        const Vector2f BA = {A.x - B.x, A.y - B.y};
        const Vector2f BC = {C.x - B.x, C.y - B.y};
        const double cross = BA.cross(BC);

        return (std::abs(cross) < epsilond) ? false : (cross < 0);
    }

    float polygon_area(const std::vector<Vector2f>& pts) {
        float area = 0;
        for (size_t i = 0; i < pts.size(); ++i) {
            const Vector2f& a = pts[i];
            const Vector2f& b = pts[(i + 1) % pts.size()];
            area += (a.x * b.y - b.x * a.y);
        }
        return area * 0.5f;
    }

    void triangulate() {
        std::vector<Vector2f> tmp = _points;

        if (polygon_area(tmp) < 0)
            std::reverse(tmp.begin(), tmp.end());

        while (tmp.size() >= 3) {
            bool earFound = false;

            for (size_t i = 0; i < tmp.size(); i++) {
                Vector2f A = tmp[i];
                Vector2f B = tmp[(i + 1) % tmp.size()];
                Vector2f C = tmp[(i + 2) % tmp.size()];
                Triangle<double> t = Triangle<double>{A, B, C};

                if (is_convex(A, B, C)) {
                    bool isEar = true;
                    for (size_t j = 0; j < tmp.size(); j++) {
                        if (j == i || j == (i + 1) % tmp.size() || j == (i + 2) % tmp.size()) continue;
                        if (t.isInside(tmp[j])) {
                            isEar = false;
                            break;
                        }
                    }

                    if (isEar) {
                        _triangles.push_back(Triangle<double>{A, B, C});
                        size_t earIndex = (i + 1) % tmp.size();
                        tmp.erase(tmp.begin() + earIndex);
                        earFound = true;
                        break;
                    }
                }
            }

            if (!earFound)
                break;
        }
    }

    // sommets en coordonnees LOCALES, construits une fois apres la
    // triangulation - aucune allocation par frame
    void buildVertices() {
        _vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        _vertices.resize(_triangles.size() * 3);

        size_t i = 0;
        for (const Triangle<double> &t : _triangles) {
            _vertices[i].position = {float(t.p1.x), float(t.p1.y)};
            _vertices[i++].color = _color;
            _vertices[i].position = {float(t.p2.x), float(t.p2.y)};
            _vertices[i++].color = _color;
            _vertices[i].position = {float(t.p3.x), float(t.p3.y)};
            _vertices[i++].color = _color;
        }
    }

    sf::Color _color;
    sf::Vector2f _position;
    sf::VertexArray _vertices;

    std::vector<Vector2f> _points;
    std::vector<Triangle<double>> _triangles;
};

/** @} */

#endif /* !SFMLPOLYGON_HPP_ */
