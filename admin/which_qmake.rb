#!/usr/bin/ruby

def check
  versions = Hash.new
  ['qmake','qmake-qt4'].each do |qmake|
    /^Using Qt version (\d\.\d\.\d)(-(.+))?/.match( `#{qmake} -v 2> /dev/null` )
    versions[qmake] = $1 unless $1.nil?
  end

  versions.each do |key, v|
    i = 1
    j = 0
    v.split( '.' ).reverse.each {|n| j += (n.to_i * i); i *= 100}
    versions[key] = j
  end
  
  return '' if versions.nil? 
  
  versions.sort {|a,b| a[1]<=>b[1]}

  versions.each do |k, v| 
    if v >= 40400
      return k
    end
    return 'toolow'
  end
end

puts check
