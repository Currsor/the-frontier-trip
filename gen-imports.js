const fs = require('fs');
const path = require('path');

const dir = path.join(__dirname, 'TypeScript', 'Blueprints');
const entryFile = path.join(__dirname, 'TypeScript', 'CurrsorGame.ts');

// 递归获取所有 .ts 文件（不带 .ts 后缀）
function getAllTsFiles(dirPath, relativePath = '') {
    let results = [];
    const list = fs.readdirSync(dirPath);
    list.forEach(file => {
        const filePath = path.join(dirPath, file);
        const relPath = path.join(relativePath, file);
        const stat = fs.statSync(filePath);
        if (stat.isDirectory()) {
            results = results.concat(getAllTsFiles(filePath, relPath));
        } else if (file.endsWith('.ts')) {
            results.push(relPath.replace(/\\/g, '/').replace(/\.ts$/, ''));
        }
    });
    return results;
}

const files = getAllTsFiles(dir);
const importLines = files.map(f => `import "./Blueprints/${f}";`).join('\n');

// 保留原有非 import 内容
let original = '';
if (fs.existsSync(entryFile)) {
    original = fs.readFileSync(entryFile, 'utf-8')
        .replace(/import\s+["'][^"']+["'];\s*\n?/g, '');
}

fs.writeFileSync(entryFile, `${importLines}\n\n${original.trim()}\n`);
console.log('已自动写入 CurrsorGame.ts');