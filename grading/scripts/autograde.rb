#!ruby

require 'octokit'
require 'csv'

# =====================================================================
#  github setup
# =====================================================================
git_token = ENV["GITHUB_TOKEN"]
unless git_token
  throw "You must set the GITHUB_TOKEN environment variable"
end
client = Octokit::Client.new(:access_token => git_token)
client.auto_paginate = true
user = client.user
git_username = user.login
class_repo_name = "msu/csci-366-fall2022"
class_repo_url = "https://github.com/#{class_repo_name}"
puts("github user id: #{user.login}")

# =====================================================================
#  helper functions
# =====================================================================

def each_student
  results = CSV.read("students.csv", headers: true)
  results.each do |row|
    yield row
  end
end

def for_each_student_dir
  each_student do |student|
    dir = "repos/#{student_dir(student)}"
    if Dir.exist? dir
      Dir.chdir dir do
        yield student["FIRST_NAME"], student["LAST_NAME"], dir
      end
    else
      puts "Directory #{dir} does not exist"
    end
  end
end

def student_dir(student)
  student["FIRST_NAME"].strip.gsub(" ", "_").downcase + "_" + student["LAST_NAME"].strip.gsub(" ", "_").downcase
end

def maven_test(pattern, output_file)
  puts `mvn -B "-Dtest=#{pattern}" test > #{output_file} 2> err.out`
end

def cmake(output_file)
    puts `cmake . > tmp.out; make > #{output_file} 2> err.out;`
end

def maybe_exec(command, output_file)
    begin
        output = `#{command}`
        File.open(output_file, 'w') do |file|
            file.write output
        end
    rescue
        puts "Could not execute #{command}"
    end
end

def pull
  puts `git pull`
end

def push_grading
  puts `git add grading/*; git commit -m "From Autograder"; git push`
end

def grading_dir_exist?
  Dir.exist? "grading"
end

def make_grading_dir
  Dir.mkdir "grading" unless Dir.exist? "grading"
end

# =====================================================================
#  command line
# =====================================================================

case ARGV[0]
when "accept_invites"
  puts("Accepting Repository Invites")
  puts("------------------")
  client.user_repository_invitations.each do |ri|
    puts "  Accepting #{ri[:id]}: #{ri[:repository][:full_name]}"
    client.accept_repo_invitation(ri[:id])
  end
when "grade"
  assignment = ARGV[1]
  filter = ARGV[2]
  case assignment
  when "chwk"
    for_each_student_dir do |first, last, dir|
      if filter and not "#{first} #{last}".downcase.include?(filter.downcase)
        next
      end
      puts "Grading #{assignment} for #{first} #{last} in #{dir}"
      pull
      make_grading_dir
      if not Dir.exist? 'c/homework'
        puts "c/homework not found..."
        next
      end
      Dir.chdir 'c/homework' do
        cmake "../../grading/homework_2_results.txt"
        maybe_exec "./homework_2", "../../grading/homework_2_results.txt"
      end
      push_grading
    end
  when "hwk3"
    for_each_student_dir do |first, last, dir|
      if filter and not "#{first} #{last}".downcase.include?(filter.downcase)
        next
      end
      puts "Grading #{assignment} for #{first} #{last} in #{dir}"
      pull
      make_grading_dir
      if not Dir.exist? 'homework_3'
        next
      end
      Dir.chdir 'homework_3' do
        #trim comments
        `cat homework_3.mips | egrep -v ';;' > tmp.mips`
        #run mips
        `java -jar ../../../../../homework_3/Mars4_5.jar tmp.mips > tmp.out`
        #diff
        `diff tmp.out ../../../../../homework_3/mips_output.txt > ../grading/homework_3_results.txt`
      end
      push_grading
    end
  when "proj1"
    for_each_student_dir do |first, last, dir|
      if filter and not "#{first} #{last}".downcase.include?(filter.downcase)
        next
      end
      puts "Grading #{assignment} for #{first} #{last} in #{dir}"
      pull
      make_grading_dir
      if not Dir.exist? 'homework_2'
        next
      end
      Dir.chdir 'battlebit' do
        cmake "../grading/project_1_results.txt"
        maybe_exec "./googletest/battleBit_checkpoint1", "../grading/project_1_results.txt"
      end
      push_grading
    end
  when "project"
    for_each_student_dir do |first, last, dir|
      if filter and not "#{first} #{last}".downcase.include?(filter.downcase)
        next
      end
      puts "Grading #{assignment} for #{first} #{last} in #{dir}"
      pull
      make_grading_dir
      if not Dir.exist? 'homework_2'
        next
      end
      Dir.chdir 'battlebit' do
        cmake "../grading/project_2_results.txt"
        maybe_exec "./googletest/battleBit_tests", "../grading/project_2_results.txt"
      end
      push_grading
    end
  else
    puts "Unknown assignment: #{assignment} (expected hwk2)"
  end
when "touch"
  for_each_student_dir do |first, last, dir|
    puts "touching file for #{first} #{last} in #{dir}"
    unless grading_dir_exist?
      puts("Grading directory does not exist!")
      next
    end
    pull
    `echo "Congrats..." > grading/autograder_is_working.txt`
    push_grading
  end
when "clone_repos"
  each_student do |student|
    student_dir = student_dir(student)
    if Dir.exist? student_dir
      puts "Directory #{student_dir} already exists, skipping..."
    else
      repo_url = student["REPO"].gsub("https://", "").strip
      if repo_url.nil? || repo_url.strip.empty?
        puts("No git URL for #{student["FIRST_NAME"]} #{student["LAST_NAME"]}")
        next
      end
      # puts("git clone https://#{git_username}:#{git_token}@#{repo_url} repos/#{student_dir}")
      `git clone https://#{git_username}:#{git_token}@#{repo_url} repos/#{student_dir}`
    end
  end
when "clear_repos"
  `rm -rf repos/*`
when "update_repos"
    for_each_student_dir do |first, last, dir|
      if filter and not "#{first} #{last}".downcase.include?(filter.downcase)
        next
      end
      pull
    end
else
  puts "Commands:
    accept_invites - accepts pending invites
    clone_repos - clones student repos to the repos directory
    clear_repos - removes all repos from the current dir
    grade <assignment> [student name] - grades the given assignment and pushes it (homework_4, project)"
end
