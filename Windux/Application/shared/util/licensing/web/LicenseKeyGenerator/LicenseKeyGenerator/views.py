from HTMLParser import HTMLParser

from django.http.response import HttpResponseForbidden
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt

from LicenseKeyGenerator import LAST_UPDATED
from LicenseKeyGenerator import VERSION
from LicenseKeyGenerator import license_key
from LicenseKeyGenerator.forms import LicenseForm
from LicenseKeyGenerator.forms import LicenseRequestForm


def require_authorization(view_function):
    def _authorize(request):
        error_message = '<h1 style="color:red;">Error! Access Forbidden</h1>'
        if request.method != 'POST':
            return HttpResponseForbidden(error_message)

        if not request.POST.get('authorized', 'False') == 'True':
            return HttpResponseForbidden(error_message)

        if not request.POST.get('login_name', None) and \
           not request.session.get('login_name', None):
            return HttpResponseForbidden(error_message)

        return view_function(request)

    return _authorize


def render_license_page(request, **kwargs):
    template = 'license_form.html'
    form = LicenseForm(initial={'timeout': 0,
                                'login_name': request.session['login_name']})

    context = {'form': form,
               'license_data': '',
               'submitted': False,
               'last_updated': LAST_UPDATED,
               'version': VERSION,
               }

    for variable, default in context.items():
        context[variable] = kwargs.get(variable, default)

    return render(request, template, context)


@csrf_exempt
@require_authorization
def license_page(request):
    if request.POST.get('login_name', False):
        request.session['login_name'] = request.POST['login_name']

    if request.POST.get('submitted', 'False') == 'True':
        request_data = request.POST.copy()
        request_data['login_name'] = request.session['login_name']

        parser = HTMLParser()

        form = LicenseForm(request_data)
        if form.is_valid():
            license_data = parser.unescape(license_key.generate_formatted_license(form.cleaned_data))

            if not request.session.get('saved', False):
                model = form.save()
                model.license_file_data = license_data
                model.save()

                request.session['saved'] = True

            return render_license_page(request,
                                       form=form,
                                       license_data=license_data,
                                       submitted=True)

        else:
            return render_license_page(request, form=form)

    request.session['saved'] = False

    return render_license_page(request)


@require_authorization
def import_request(request):
    form = LicenseRequestForm()

    if request.FILES.get('request_file', False):
        form = LicenseRequestForm(request.POST, request.FILES)

        if form.is_valid():
            request_file = form.cleaned_data['request_file']
            request_text = request_file.read()

            license_request = license_key.LicenseRequest(request_text)
            initial_data = license_request.as_dictionary()
            initial_data['login_name'] = request.session['login_name']

            license_form = LicenseForm(initial=initial_data)
            return render_license_page(request,
                                       form=license_form,
                                       submitted=False)

    return render(request, 'import_request.html', {'form': form})


@require_authorization
def send_email(request):
    form = LicenseForm(request.POST)

    if not form.is_valid():
        return license_page(request)

    license_data = license_key.generate_formatted_license(form.cleaned_data)
    email_address = (form.cleaned_data['email_address'],)
    purchased = form.cleaned_data['license_type'] == 1
    timeout = '0'

    if not purchased:
        timeout = form.cleaned_data['timeout']

    license_key.send_email(license_data, email_address, purchased=purchased, timeout=timeout)

    return license_page(request)
