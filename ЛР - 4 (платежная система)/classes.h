
#pragma once

#include <iostream>
#include <sstream>
#include <utility>
#include <algorithm>
#include <memory>
#include <vector>
#include <map>


//---------- класс Product ---------
class ProductInfo {
public:
    explicit ProductInfo(std::string info) : m_info(std::move(info)) {};
    std::string get_info() { return m_info; }

private:
    std::string m_info;
};


class Product {
public:
    explicit Product(ProductInfo& product_info) : m_product_info(product_info) {};
    virtual std::string get_info() const = 0;
    virtual double get_cost() const = 0;

protected:
    ProductInfo& m_product_info;
};


class WeightProduct : public Product {
public:
    WeightProduct(ProductInfo& product_info, double cost_per_kg) :
        Product(product_info), m_cost_per_kg(cost_per_kg) {};
    double get_cost() const override { return m_cost_per_kg; }
    std::string get_info() const override { 
        std::stringstream s;
        s << get_cost(); 
        return m_product_info.get_info() + " : " + s.str() + " per kg"; }

private:
    double m_cost_per_kg;
};


class AmountProduct : public Product {
public:
    AmountProduct(ProductInfo& product_info, double cost_per_one) :
        Product(product_info), m_cost_per_one(cost_per_one) {};
    double get_cost() const override { return m_cost_per_one; }
    std::string get_info() const override {
        std::stringstream s;
        s << get_cost(); 
        return m_product_info.get_info() + " : " + s.str() + " per one"; }

private:
    double m_cost_per_one;
};



//---------- класс Position ---------

class Position {
public:
    explicit Position(std::unique_ptr<Product>&& ptr_product) : m_ptr_product(std::move(ptr_product)) {};
    std::unique_ptr<Product>& get_ptr_product() { return m_ptr_product; }
    virtual double get_cost() = 0;
    virtual double get_quantity() = 0;

protected:
    std::unique_ptr<Product> m_ptr_product;
};


class AmountPosition : public Position {
public:
    AmountPosition(std::unique_ptr<AmountProduct>&& ptr_product, size_t amount) :
        Position(std::move(ptr_product)), m_amount(amount) {};
    double get_cost() override { return m_amount * m_ptr_product->get_cost(); }
    double get_quantity() override { return m_amount; }

private:
    size_t m_amount;
};


class WeightPosition : public Position {
public:
    WeightPosition(std::unique_ptr<WeightProduct>&& ptr_product, double weight) :
        Position(std::move(ptr_product)), m_weight(weight) {};
    double get_cost() override { return m_weight * m_ptr_product->get_cost(); }
    double get_quantity() override { return m_weight; }

private:
    double m_weight;
};


//----------- класс Order -----------

class Order {
public:
    Order() = default;

    void add_position(std::unique_ptr<Position> ptr_pos) {

        auto compare = [&ptr_pos](std::unique_ptr<Position>& ptr_pos_resource) {
            return ptr_pos_resource->get_ptr_product()->get_info() == ptr_pos->get_ptr_product()->get_info();
        };

        auto repeated_position = std::find_if(m_ptr_positions.begin(), m_ptr_positions.end(), compare);
        if (repeated_position != m_ptr_positions.end()) 
        {
            *repeated_position = std::move(ptr_pos);
        }
        else 
        { 
            m_ptr_positions.push_back(std::move(ptr_pos)); 
        }
    }

    double get_cost() {
        double sum = 0;
        for (auto& el : m_ptr_positions) {
            sum += el->get_cost();
        }
        return sum;
    }

    void get_info() {
        if (empty()) {
            std::cout << "В заказе ничего нету!" << std::endl;
        }
        else {
            std::cout << "Информация о заказе:" << std::endl;
            std::cout << "-------------------------------" << std::endl;
            for (auto& el : m_ptr_positions) {
                std::cout << el->get_ptr_product()->get_info() << std::endl;
                std::cout << "\tQuantity: " << el->get_quantity() << std::endl;
                std::cout << "\tCost: " << el->get_cost() << std::endl;
            }
            std::cout << "Total cost: " << get_cost() << std::endl;
            std::cout << "--------------------------------" << std::endl;
        }
    }

    bool empty() { return m_ptr_positions.empty(); }

private:
    std::vector<std::unique_ptr<Position>> m_ptr_positions;
    void clear() { m_ptr_positions.clear(); }
    friend class Client;
};


//--------- класс Client ---------

class Client {
    class Balance {
    public:
        void add_money(double rubles) { m_rubles += rubles; }
        bool waste_money(double rubles) {
            if ( m_rubles > rubles || std::fabs(m_rubles - rubles) < std::numeric_limits<double>::epsilon() ) {
                m_rubles -= rubles;
                return true; 
            }
            else { return false; }
        }
        double get_balance() { return m_rubles; }
    private:
        double m_rubles = 0;
    };

public:
    void earn_dollars(double dollars) { m_balance.add_money(dollars * 56); }
    void earn_euros(double euros) { m_balance.add_money(euros * 60); }
    void earn_bitcoins(double bitcoins) { m_balance.add_money(bitcoins * 56 * 21000); }
    void earn_rubles(double rubles) { m_balance.add_money(rubles); }
    bool pay_and_receive_order(Order& ord) {
        if (ord.empty()) {
            std::cout << "The order is empty! " << "Remaining on ruble account: " << m_balance.get_balance() << std::endl << std::endl;
            return false;
        }
        if ( !(m_balance.waste_money( ord.get_cost())) ) {
            std::cout << "Not enough money to pay for order! " << "Remaining on ruble account: " << m_balance.get_balance() << std::endl << std::endl;
            return false;
        }
        else {
            ord.clear();
            std::cout << "The order is received! The purchase is successful! " << "Remaining on ruble account: " << m_balance.get_balance() << std::endl << std::endl;
            return true;
        }
    }

private:
    Balance m_balance;
};


//--------- класс Pricebase ---------

class PriceBase {
public:
    static PriceBase& instance() {
        static PriceBase obj;
        return obj;
    }
    WeightProduct get_product_weight_price(const std::string& name) { return m_products_weight_price.at(name); }
    AmountProduct get_product_amount_price(const std::string& name) { return m_products_amount_price.at(name); }
private:
    std::map<std::string, ProductInfo> m_products_info;
    std::map<std::string, WeightProduct> m_products_weight_price;
    std::map<std::string, AmountProduct> m_products_amount_price;
protected:
    PriceBase() {
        m_products_info = {
                { "Apple", ProductInfo("Green sweet apple") },
                { "Nuts", ProductInfo("Macadamia nut") },
                { "Pencil", ProductInfo("Red pencil with rubber") },
                { "Chair", ProductInfo("Wooden chair with armrests") },
        };
        m_products_weight_price = {
                { "Apple", WeightProduct(m_products_info.at("Apple"), 11.3) },
                { "Nuts", WeightProduct(m_products_info.at("Nuts"), 34.2) },
        };
        m_products_amount_price = {
                { "Pencil", AmountProduct(m_products_info.at("Pencil"), 5.7) },
                { "Chair", AmountProduct(m_products_info.at("Chair"), 75) },
        };
    }
};