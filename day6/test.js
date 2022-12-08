const input = require('fs').readFileSync('./data.txt', 'utf8');
for(let i = 3; i < input.length; i++) {
  let found = 0;
  for(let j = i - 3; !found && j <= i; j++) {
    for(let k = j + 1; !found && k <= i; k++) {
      if(input[j] == input[k]) {
        found = 1;
        break;
      }
    }
  }
  if(!found) {
    console.log(`4-seq (${input.substr(i - 3, 4)}) found at ${i}`);
    break;
  }
}