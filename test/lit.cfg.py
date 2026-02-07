import lit.formats
import os

config.name = 'AST-PRINTER'
config.test_format = lit.formats.ShTest(execute_external=True)

config.suffixes = ['.cpp', '.c', '.ll']

config.test_source_root = os.path.dirname(__file__)

if config.llvm_version:
    config.available_features.add(f"llvm-{config.llvm_version}")

# Substitutions
config.substitutions.append(('%clang-ast-printer', config.llvm_ast_printer_binary))
config.substitutions.append(('%filecheck', config.filecheck_binary))
config.substitutions.append(('%llvm-profdata', config.llvm_profdata_exe))
config.substitutions.append(('%llvm-cov', config.llvm_cov_exe))

if config.enable_coverage.upper() in ['ON', 'YES', 'TRUE', '1']:
    config.environment['LLVM_PROFILE_FILE'] = os.path.join(config.test_exec_root, "test-%p.profraw")
