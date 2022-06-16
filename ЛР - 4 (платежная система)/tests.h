#pragma once

#include "classes.h"

void test() {
    auto& base = PriceBase::instance();
    Order ord;
    std::cout << std::boolalpha << ord.empty() << std::endl;


    auto pos1 = WeightPosition(std::make_unique<WeightProduct>(base.get_product_weight_price("Apple")), 0.5);
    ord.add_position(std::make_unique<WeightPosition>(std::move(pos1)));
    ord.get_info();
    std::cout << std::endl;

    auto pos2 = AmountPosition(std::make_unique<AmountProduct>(base.get_product_amount_price("Pencil")), 2);
    ord.add_position(std::make_unique<AmountPosition>(std::move(pos2)));
    ord.get_info();
    std::cout << std::endl;

    auto pos3 = WeightPosition(std::make_unique<WeightProduct>(base.get_product_weight_price("Apple")), 2);
    ord.add_position(std::make_unique<WeightPosition>(std::move(pos3)));
    ord.get_info();
    std::cout << std::endl;


    Client Ivan;
    Ivan.earn_rubles(10);
    Ivan.pay_and_receive_order(ord); // недостаточно средств
    Ivan.earn_dollars(100);
    Ivan.pay_and_receive_order(ord); // успешная оплата
    Ivan.earn_bitcoins(1);
    Ivan.pay_and_receive_order(ord); // заказ уже был оплачен ранее
}
