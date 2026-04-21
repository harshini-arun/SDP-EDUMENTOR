from flask import Flask, render_template, request, redirect, session, url_for
import subprocess
import os

app = Flask(__name__)
app.secret_key = "mentor_secret"

# --- 1. PATHS TO C EXECUTABLES ---
MENTOR_EXE = "../backend/mentor_module"
MENTEE_EXE = "../backend/mentee_module"
ADMIN_EXE  = "../backend/admin_module"

# --- 2. HELPER FUNCTIONS TO TALK TO C ---
def run_mentor_c(args):
    result = subprocess.run([MENTOR_EXE] + args, capture_output=True, text=True)
    return result.stdout.strip()

def run_mentee_c(args):
    result = subprocess.run([MENTEE_EXE] + args, capture_output=True, text=True)
    return result.stdout.strip()

def run_admin_c(args):
    result = subprocess.run([ADMIN_EXE] + args, capture_output=True, text=True)
    return result.stdout.strip()

# --- 3. LANDING PAGE (ROLE SELECTION) ---
@app.route('/')
def index():
    return render_template('index.html')

# --- 4. MENTOR MODULE ROUTES ---
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        mid = request.form.get('mentorID')
        pwd = request.form.get('password')
        res = run_mentor_c(['login', mid, pwd])
        
        if res != "0":
            data = res.split(',')
            session['role'] = 'mentor'
            session['mentorID'] = mid
            session['name'] = data[1]
            session['dept'] = data[2]
            session['designation'] = data[3]
            session['email'] = data[4]
            session['phone'] = data[5]
            return redirect(url_for('dashboard'))
        return "Login Failed. Invalid Mentor ID or Password."
    return render_template('login.html')

@app.route('/dashboard')
def dashboard():
    if session.get('role') != 'mentor': return redirect('/login')
    return render_template('mentor.html', 
                           name=session.get('name'), 
                           mid=session.get('mentorID'),
                           dept=session.get('dept'),
                           designation=session.get('designation'),
                           email=session.get('email'),
                           phone=session.get('phone'))

@app.route('/mentor_mentees')
def view_mentees():
    if session.get('role') != 'mentor': return redirect('/login')
    mentees_list = []
    res = run_mentor_c(['list', str(session['mentorID'])])
    if res and res != "0":
        for line in res.split('\n'):
            if "|" in line:
                parts = line.split('|')
                mentees_list.append({
                    'id': parts[0], 'name': parts[1], 'reg': parts[2],
                    'cgpa': parts[3], 'attn': parts[4], 'remarks': parts[5]
                })
    return render_template('mentor_mentees.html', mentees=mentees_list)

@app.route('/mentor_meetings')
def view_meetings():
    if session.get('role') != 'mentor': return redirect('/login')
    
    # Get Mentees for the dropdown
    mentees_list = []
    res_m = run_mentor_c(['list', str(session['mentorID'])])
    if res_m and res_m != "0":
        for line in res_m.split('\n'):
            if "|" in line:
                p = line.split('|')
                mentees_list.append({'id': p[0], 'name': p[1], 'reg': p[2]})

    # Get Meetings for the queue list
    meetings_list = []
    res_mt = run_mentor_c(['meeting_list', str(session['mentorID'])])
    if res_mt and "|" in res_mt:
        for line in res_mt.split('\n'):
            if "|" in line:
                parts = line.split('|')
                meetings_list.append({
                    'id': parts[0], 'menteeID': parts[1], 'name': parts[2],
                    'reg': parts[3], 'topic': parts[4], 'datetime': parts[5], 'status': parts[6]
                })
    return render_template('mentor_meetings.html', mentees=mentees_list, meetings=meetings_list)

    # --- MENTOR CRUD OPERATIONS ---

@app.route('/mentees/add', methods=['POST'])
def add_mentee():
    if session.get('role') != 'mentor': return redirect('/login')
    
    name = request.form.get('name')
    reg = request.form.get('regNo')
    cgpa = request.form.get('cgpa')
    attn = request.form.get('attendance')
    
    # Action 'add' in mentor_module.c needs 5 args: name, reg, cgpa, attn, mentorID
    run_mentor_c(['add', name, reg, cgpa, attn, str(session['mentorID'])])
    return redirect(url_for('view_mentees'))

@app.route('/mentor/mentee/update/<int:id>')
def update_mentee_page(id):
    if session.get('role') != 'mentor': return redirect('/login')
    
    # Call C: find mentee details to pre-fill the form
    res = run_mentor_c(['find', str(id)])
    
    if res and res != "0":
        parts = res.split('|')
        mentee = {
            'id': parts[0], 'name': parts[1], 'reg': parts[2],
            'cgpa': parts[3], 'attn': parts[4], 'remarks': parts[5], 'notes': parts[6]
        }
        return render_template('mentor_update.html', m=mentee)
    return "Mentee not found."

@app.route('/mentor/mentee/save_update', methods=['POST'])
def save_update():
    if session.get('role') != 'mentor': return redirect('/login')
    
    mid = request.form.get('id')
    cgpa = request.form.get('cgpa')
    attn = request.form.get('attendance')
    remarks = request.form.get('remarks')
    notes = request.form.get('notes')
    
    # Action 'update' in mentor_module.c needs: id, cgpa, attn, remarks, notes
    run_mentor_c(['update', str(mid), str(cgpa), str(attn), remarks, notes])
    return redirect(url_for('view_mentees'))

@app.route('/mentor/mentee/delete/<int:id>')
def delete_mentee(id):
    if session.get('role') != 'mentor': return redirect('/login')
    
    # Action 'delete' in mentor_module.c
    run_mentor_c(['delete', str(id)])
    return redirect(url_for('view_mentees'))

# --- MENTOR MEETING OPERATIONS ---

@app.route('/meetings/add', methods=['POST'])
def add_meeting():
    if session.get('role') != 'mentor': return redirect('/login')
    
    mentee_id = request.form.get('menteeID')
    dt = request.form.get('datetime')
    topic = request.form.get('topic')

    # 1. Get mentee name and reg from C first
    res = run_mentor_c(['find', str(mentee_id)])
    mentee_name, mentee_reg = "Unknown", ""
    if res and "|" in res:
        parts = res.split('|')
        mentee_name = parts[1]
        mentee_reg  = parts[2]

    # 2. Add the meeting
    run_mentor_c(['meeting_add', str(session['mentorID']), str(mentee_id),
                 mentee_name, mentee_reg, topic, dt])
    return redirect(url_for('view_meetings'))

@app.route('/meetings/complete/<int:meeting_id>')
def complete_meeting(meeting_id):
    if session.get('role') != 'mentor': return redirect('/login')
    run_mentor_c(['meeting_complete', str(meeting_id)])
    return redirect(url_for('view_meetings'))

@app.route('/meetings/cancel/<int:meeting_id>')
def cancel_meeting(meeting_id):
    if session.get('role') != 'mentor': return redirect('/login')
    run_mentor_c(['meeting_cancel', str(meeting_id)])
    return redirect(url_for('view_meetings'))

# --- 5. MENTEE MODULE ROUTES ---
@app.route('/mentee/login', methods=['GET', 'POST'])
def mentee_login():
    if request.method == 'POST':
        mid = request.form.get('menteeID')
        pwd = request.form.get('password')
        res = run_mentee_c(['login', mid, pwd])
        
        if res != "0":
            parts = res.split('|')
            session['role'] = 'mentee'
            session['menteeID'] = parts[1]
            session['menteeName'] = parts[2]
            session['myMentorID'] = parts[3]
            return redirect(url_for('mentee_dashboard'))
        return "Login Failed. Invalid Mentee ID or Password."
    return render_template('mentee_login.html')

@app.route('/mentee/dashboard')
def mentee_dashboard():
    if session.get('role') != 'mentee': return redirect('/mentee/login')
    return render_template('mentee_dashboard.html', name=session['menteeName'])

@app.route('/mentee/details')
def mentee_details():
    if session.get('role') != 'mentee': return redirect('/mentee/login')
    res = run_mentee_c(['view_details', str(session['menteeID'])])
    p = res.split('|')
    mentee = {'id': p[0], 'name': p[1], 'reg': p[2], 'dept': p[3], 'year': p[4], 
              'email': p[5], 'phone': p[6], 'cgpa': p[7], 'attn': p[8], 'remarks': p[9]}
    return render_template('mentee_details.html', m=mentee)

@app.route('/mentee/mentor')
def mentee_mentor():
    if session.get('role') != 'mentee': return redirect('/mentee/login')
    res = run_mentee_c(['view_mentor', str(session['myMentorID'])])
    p = res.split('|')
    mentor = {'name': p[0], 'dept': p[1], 'desig': p[2], 'email': p[3], 'phone': p[4]}
    return render_template('mentor_details_view.html', m=mentor)

@app.route('/mentee/meetings', methods=['GET', 'POST'])
def mentee_meetings():
    if session.get('role') != 'mentee': return redirect('/mentee/login')
    if request.method == 'POST':
        topic = request.form.get('topic')
        dt = request.form.get('datetime')
        run_mentee_c(['request_meeting', str(session['myMentorID']), str(session['menteeID']), topic, dt])
        return redirect(url_for('mentee_meetings'))

    res = run_mentee_c(['list_meetings', str(session['menteeID'])])
    meetings = []
    if res:
        for line in res.split('\n'):
            if "|" in line:
                p = line.split('|')
                meetings.append({'id': p[0], 'topic': p[1], 'date': p[2], 'status': p[3]})
    return render_template('mentee_meetings.html', meetings=meetings)


# --- 7. UTILITY ROUTES ---
@app.route('/logout')
def logout():
    session.clear()
    return redirect('/')

# --- ADMIN MODULE ROUTES ---

@app.route('/admin/login', methods=['GET', 'POST'])
def admin_login():
    if request.method == 'POST':
        user = request.form.get('username')
        pwd = request.form.get('password')
        res = run_admin_c(['login', user, pwd])
        if res == "1":
            session['role'] = 'admin'
            return redirect(url_for('admin_dashboard'))
        return "Invalid Admin Credentials"
    return render_template('admin_login.html')

@app.route('/admin/dashboard')
def admin_dashboard():
    if session.get('role') != 'admin': return redirect('/admin/login')
    # Gets mentor_count|mentee_count from C
    res = run_admin_c(['stats'])
    stats = res.split('|') if res else [0, 0]
    return render_template('admin_dashboard.html', mentor_count=stats[0], mentee_count=stats[1])

@app.route('/admin/mentors')
def admin_mentors():
    if session.get('role') != 'admin': return redirect('/admin/login')
    mentors = []
    res = run_admin_c(['list_mentors']) # BST Inorder Traversal
    if res:
        for line in res.split('\n'):
            if "|" in line:
                p = line.split('|')
                mentors.append({'id': p[0], 'name': p[1], 'dept': p[2], 'desig': p[3], 'email': p[4]})
    return render_template('admin_mentors.html', mentors=mentors)

@app.route('/admin/mentors/add', methods=['GET', 'POST'])
def admin_add_mentor():
    if session.get('role') != 'admin': return redirect('/admin/login')
    if request.method == 'POST':
        mid = request.form.get('mentorID')
        name = request.form.get('name')
        dept = request.form.get('dept')
        desig = request.form.get('desig')
        email = request.form.get('email')
        phone = request.form.get('phone')
        pwd = request.form.get('password')
        run_admin_c(['add_mentor', mid, name, dept, desig, email, phone, pwd])
        return redirect(url_for('admin_mentors'))
    return render_template('admin_add_mentor.html')

@app.route('/admin/mentor/<int:mid>')
def admin_mentor_profile(mid):
    if session.get('role') != 'admin': return redirect('/admin/login')
    # 1. Search Mentor via BST
    m_res = run_admin_c(['search_mentor', str(mid)])
    if m_res == "0": return "Mentor Not Found"
    p = m_res.split('|')
    mentor_info = {'id': p[0], 'name': p[1], 'dept': p[2], 'desig': p[3], 'email': p[4], 'phone': p[5]}
    # 2. Get Mentees using existing Mentor Module Logic
    mentees_list = []
    res_mentees = run_mentor_c(['list', str(mid)]) 
    if res_mentees and res_mentees != "0":
        for line in res_mentees.split('\n'):
            if "|" in line:
                pts = line.split('|')
                mentees_list.append({'id': pts[0], 'name': pts[1], 'reg': pts[2], 'cgpa': pts[3], 'attn': pts[4]})
    return render_template('admin_mentor_profile.html', mentor=mentor_info, mentees=mentees_list)

@app.route('/admin/reports')
def admin_reports():
    if session.get('role') != 'admin': return redirect('/admin/login')
    # 1. Global Stats
    res = run_admin_c(['global_report'])
    p = res.split('|') if res else [0, 0, 0]
    global_stats = {'count': p[0], 'avg_cgpa': p[1], 'avg_attn': p[2]}
    # 2. Mentor List for drill-down
    mentors = []
    m_res = run_admin_c(['list_mentors'])
    if m_res:
        for line in m_res.split('\n'):
            if "|" in line:
                pts = line.split('|')
                mentors.append({'id': pts[0], 'name': pts[1], 'dept': pts[2]})
    return render_template('admin_reports.html', stats=global_stats, mentors=mentors)

@app.route('/admin/reports/mentor/<int:mid>')
def admin_mentor_report(mid):
    if session.get('role') != 'admin': return redirect('/admin/login')
    # Get Mentor Name
    m_info = run_admin_c(['search_mentor', str(mid)]).split('|')
    # Get Averages from C
    res = run_admin_c(['mentor_report', str(mid)])
    p = res.split('|') if res else [0, 0, 0]
    # Convert to numbers for Jinja math safety
    m_stats = {'count': int(p[0]), 'avg_cgpa': float(p[1]), 'avg_attn': float(p[2]), 'name': m_info[1]}
    return render_template('admin_mentor_report.html', s=m_stats)
if __name__ == '__main__':
    app.run(debug=True)