ARGF.each do |line|
  puts "#{line.split("\t")[1]} #{line.split("\t")[3]}"
end
