#!/usr/bin/python

def remove_nested_parentheses(line: str) -> str:
    depth = 0
    for index, character in enumerate(line):
        if character == '(':
            if depth == 0:
                opening_index = index
            depth += 1
        elif character == ')':
            if depth == 0:
                raise RuntimeError(f"Parentheses do not match: {line}")

            depth -= 1
            if depth == 0:
                return remove_nested_parentheses(line[:opening_index] + line[index + 1:])

    return ' '.join(line.split())


def delete_comments(line: str) -> str:
    comment_index = line.find("{")
    if comment_index == -1:
        return remove_nested_parentheses(line)

    comment_closer_index = line.find('}', comment_index + 1)
    if comment_closer_index == -1:
        return delete_comments(line[:comment_index])
    else:
        return delete_comments(line[:comment_index] + " " + line[comment_closer_index + 1:])
