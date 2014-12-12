/* Copyright 2014 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "backgroundWidget.h"

#include <QtCore/QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

using namespace trikGui;

BackgroundWidget::BackgroundWidget(QString const &configPath
		, QString const &startDirPath
		, QWidget *parent)
	: QWidget(parent)
	, mController(configPath, startDirPath)
	, mBatteryIndicator(mController.brick())
	, mStartWidget(mController, configPath)
{
	setWindowState(Qt::WindowFullScreen);

	mMainLayout.setSpacing(10);

	mBatteryIndicator.setStyleSheet("font: 12px");

	mStatusBarLayout.addWidget(&mBatteryIndicator);
	addMainWidget(mStartWidget);

	mMainLayout.addLayout(&mStatusBarLayout);
	mMainLayout.addLayout(&mMainWidgetsLayout);

	setLayout(&mMainLayout);

	connect(&mMainWidgetsLayout, SIGNAL(currentChanged(int)), this, SLOT(renewFocus()));
	connect(&mController, SIGNAL(addRunningWidget(trikKernel::MainWidget &)), this, SLOT(addMainWidget(trikKernel::MainWidget &)));
	connect(&mController, SIGNAL(closeRunningWidget(trikKernel::MainWidget &)), this, SLOT(closeMainWidget(trikKernel::MainWidget &)));
	connect(&mController, SIGNAL(brickStopped()), this, SLOT(refresh()));
}

void BackgroundWidget::addMainWidget(trikKernel::MainWidget &widget)
{
	// If the widget has layout, remove its margins because main widgets layout has its own margins.
	QLayout *layout = widget.layout();
	if (layout != nullptr) {
		layout->setContentsMargins(0, 0, 0, 0);
	}

	int const index = mMainWidgetsLayout.addWidget(&widget);
	mMainWidgetsLayout.setCurrentIndex(index);

	connect(&widget, SIGNAL(newWidget(trikKernel::MainWidget &)), this, SLOT(addMainWidget(trikKernel::MainWidget &)));
}

void BackgroundWidget::closeMainWidget(trikKernel::MainWidget &widget)
{
	mMainWidgetsLayout.removeWidget(&widget);
}

void BackgroundWidget::renewFocus()
{
	// When current widget in main widgets layout changed, we should set focus properly.

	trikKernel::MainWidget *currentWidget = dynamic_cast<trikKernel::MainWidget *>(mMainWidgetsLayout.currentWidget());

	if (currentWidget != nullptr) {
		currentWidget->renewFocus();
	}
}

void BackgroundWidget::refresh()
{
	for (auto const widget : QApplication::allWidgets()) {
		widget->update();
	}
}

