import enum


class AstSymbols(str, enum.Enum):
    expression = 'expression'
    identifier = 'identifier'
    array_index = 'array_index'
    arguments = 'arguments'
    operator = 'operator'
    binary_exp = 'binary_exp'
    not_expression = 'not_expression'
    cast_expression = 'cast_expression'
    record_initialization = 'record_initialization'
    assign_variable = 'assign_variable'
    push_record_field = 'push_record_field'
    push_expression_field = 'push_expression_field'
    push_expression_index = 'push_expression_index'
    function_call= 'function_call'
    type_decl='type_decl'
    array_decl = 'array_decl'
    array_size_declaration = 'array_size_declaration'
    array_decl_expression = 'array_decl_expression'
    varlist = 'varlist'
    return_type_decl = 'return_type_decl'
    fbody = 'fbody'
    fname = 'fname'
    f_statement = 'f_statement'
    statement = 'statement'
    return_block = 'return_block'
    var_param = 'var_param'
    if_clause = 'if_clause'
    elif_clause = 'elif_clause'
    else_clause = 'else_clause'
    if_body = 'if_body'
    if_block = 'if_block'
    variable_decl = 'variable_decl'
    function_decl = 'function_decl'
    while_block = 'while_block'
    while_body = 'while_body'
    for_block = 'for_block'
    for_body = 'for_body'
    assign_array_index = 'assign_array_index'
    assign_record_field = 'assign_record_field'
    break_statement = 'break_statement'
    continue_statement = 'continue_statement'
    record_type_decl = 'record_type_decl'
    init_expression = 'init_expression'
    stop_expression = 'stop_expression'
    iter_expression = 'iter_expression'
    expression_list = 'expression_list'
    record_name = 'record_name'
    record_arguments = 'record_arguments'
    record_var_list = 'record_var_list'
    record_var_param = 'record_var_param'
    bit_not_expression = 'bit_not_expression'
    scalar_var_declaration = 'scalar_var_declaration'
    array_var_declaration = 'array_var_declaration'




