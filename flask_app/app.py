from flask import Flask, render_template, request, redirect, session, url_for
import subprocess
import os

app = Flask(__name__)
app.secret_key = "mentor_secret"

# Path to the compiled C executable
C_EXECUTABLE = "../backend/mentor_module"

def run_c_query(args):
    result = subprocess.run([C_EXECUTABLE] + args, capture_output=True, text=True)
    return result.stdout.strip()

@app.route('/')
def index():
    return render_template('login.html')

@app.route('/login', methods=['POST'])
def login():
    mid = request.form.get('mentorID')
    pwd = request.form.get('password')
    
    # We call the C program. 
    # Logic: C prints "1,Name,Dept,Designation,Email,Phone" on success
    res = run_c_query(['login', mid, pwd])
    
    if res != "0":
        data = res.split(',')
        session['mentorID'] = mid
        session['name'] = data[1]
        session['dept'] = data[2]
        session['designation'] = data[3]
        session['email'] = data[4]
        session['phone'] = data[5]
        return redirect(url_for('dashboard'))
    else:
        return "Login Failed. Invalid ID or Password."

@app.route('/dashboard')
def dashboard():
    if 'mentorID' not in session:
        return redirect('/')
    
    # These values come from the session (set during login)
    return render_template('mentor.html', 
                           name=session.get('name'), 
                           mid=session.get('mentorID'),
                           dept=session.get('dept', 'Information Technology'), # Example default
                           designation=session.get('designation', 'Assistant Professor'),
                           email=session.get('email', 'mentor@university.edu'),
                           phone=session.get('phone', '+1 234 567 890'))

@app.route('/mentor_mentees')
def view_mentees():
    if 'mentorID' not in session:
        return redirect('/')

    # 1. Initialize the list as empty so Python knows it exists
    mentees_list = []

    # 2. Call your C program to get the list of mentees
    # We pass the action 'list' and the mentor's ID
    res = run_c_query(['list', str(session['mentorID'])])

    # 3. Process the raw string from C into a Python List
    if res and res != "0":
        lines = res.split('\n')
        for line in lines:
            if "|" in line:  # Check if the line actually contains data
                parts = line.split('|')
                # We create a dictionary for each mentee to make it easy for HTML to read
                mentee_data = {
                    'id': parts[0],
                    'name': parts[1],
                    'reg': parts[2],
                    'cgpa': parts[3],
                    'attn': parts[4],
                    'remarks': parts[5]
                }
                mentees_list.append(mentee_data)
    return render_template('mentor_mentees.html', mentees=mentees_list)

@app.route('/mentor_meetings')
def view_meetings():
    if 'mentorID' not in session:
        return redirect('/')
        
    # We need to send the mentee list here too so the "Schedule Meeting" 
    # dropdown has names in it
    mentees_list = []
    res = run_c_query(['list', str(session['mentorID'])])
    
    if res and res != "0":
        for line in res.split('\n'):
            if "|" in line:
                parts = line.split('|')
                mentees_list.append({'id': parts[0], 'name': parts[1], 'reg': parts[2]})

    return render_template('mentor_meetings.html', mentees=mentees_list)

@app.route('/mentor/mentee/update/<int:id>')
def update_mentee_page(id):
    if 'mentorID' not in session: return redirect('/')
    
    # Call C: find mentee details to pre-fill the form
    res = run_c_query(['find', str(id)])
    
    if res and res != "0":
        parts = res.split('|')
        mentee = {
            'id': parts[0], 'name': parts[1], 'reg': parts[2],
            'cgpa': parts[3], 'attn': parts[4], 'remarks': parts[5], 'notes': parts[6]
        }
        return render_template('mentor_update_mentee.html', m=mentee)
    return "Mentee not found."

@app.route('/mentor/mentee/save_update', methods=['POST'])
def save_update():
    mid = request.form.get('id')
    cgpa = request.form.get('cgpa')
    attn = request.form.get('attendance')
    remarks = request.form.get('remarks')
    notes = request.form.get('notes')
    
    # args: update, id, cgpa, attendance, remarks, notes
    run_c_query(['update', str(mid), str(cgpa), str(attn), remarks, notes])
    
    return redirect(url_for('view_mentees'))

@app.route('/mentees/add', methods=['POST'])
def add_mentee():
    name = request.form.get('name')
    reg = request.form.get('regNo')
    cgpa = request.form.get('cgpa')
    attn = request.form.get('attendance')
    run_c_query(['add', name, reg, cgpa, attn, session['mentorID']])
    return redirect(url_for('view_mentees'))

@app.route('/logout')
def logout():
    session.clear()
    return redirect('/')

if __name__ == '__main__':
    app.run(debug=True)