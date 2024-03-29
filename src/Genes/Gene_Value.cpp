#include "Genes/Gene_Value.h"

#include <string>
#include <map>

#include "Utility/String.h"
#include "Utility/Random.h"

Gene_Value::Gene_Value(const std::string& name, const double initial_value, const double mutation_size) noexcept :
    current_value(initial_value),
    identifier(name),
    mutation_amount(mutation_size)
{
}

double Gene_Value::value() const noexcept
{
    return current_value;
}

double& Gene_Value::value() noexcept
{
    return current_value;
}

std::string Gene_Value::name() const noexcept
{
    return identifier;
}

void Gene_Value::load_from_map(const std::map<std::string, std::string>& properties)
{
    current_value = String::to_number<double>(properties.at(name()));
}

void Gene_Value::write_to_map(std::map<std::string, std::string>& properties) const noexcept
{
    properties[name()] = std::to_string(current_value);
}

void Gene_Value::mutate() noexcept
{
    current_value += Random::random_laplace(mutation_amount);
}
